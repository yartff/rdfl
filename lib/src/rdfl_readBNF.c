#include		<stdlib.h>
#include		<string.h>
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"
#include		"rdfl_readBNF.h"
#include		"rdfl_local.h"

#define		OP_RULE			"::="
#define		OP_RULE_END		";"
#define		OP_OR			"|"
#define		OP_EXPR_BEG		"["
#define		OP_EXPR_END		"]"
#define		OP_GRP_BEG		"("
#define		OP_GRP_END		")"
static int _link_exprs(t_rdfl_bnf *tmp, tl_orexpr *);

t_rdfl_bnf *
_seek_target(t_rdfl_bnf *bnf, char *id) {
  while (bnf) {
    if (!strcmp(bnf->identifier, id))
      return (bnf);
    bnf = bnf->next;
  }
  return (NULL);
}

static
t_rdfl_bnf *
_seek_target_free(t_rdfl_bnf *bnf, char *id) {
  bnf = _seek_target(bnf, id);
  free(id);
  return (bnf);
}

static
int
_link_target(t_rdfl_bnf *tmp, tl_fact *facts) {
  while (facts) {
    if (facts->type == FACT_RULE) {
      facts->type = FACT_RULE_LINKED;
      if (!(facts->target = (void *)_seek_target_free(tmp, ((char *)facts->target))))
	return (EXIT_FAILURE);
    }
    else if (facts->type >= FACT_EXPRS) {
      if (_link_exprs(tmp, ((tl_orexpr *)facts->target)) == EXIT_FAILURE)
	return (EXIT_FAILURE);
    }
    facts = facts->next;
  }
  return (EXIT_SUCCESS);
}

static
int
_link_exprs(t_rdfl_bnf *tmp, tl_orexpr *expr) {
  while (expr) {
    if (_link_target(tmp, expr->factors) == EXIT_FAILURE)
      return (EXIT_FAILURE);
    expr = expr->next;
  }
  return (EXIT_SUCCESS);
}

static
int
_link_factors(t_rdfl_bnf *tmp) {
  t_rdfl_bnf	*bnf = tmp;

  while (bnf) {
    if (_link_exprs(tmp, bnf->exprs) == EXIT_FAILURE)
      return (EXIT_FAILURE);
    bnf = bnf->next;
  }
  return (EXIT_SUCCESS);
}

static tl_orexpr		*_read_or_expression(t_rdfl *obj);
static void			_free_rule(tl_orexpr *exprs);

static
tl_orexpr *
_read_orloop(t_rdfl *obj) {
  tl_orexpr	*target, *loop;

  if ((target = _read_or_expression(obj)) == NULL)
    return (NULL);
  target->next = NULL;
  loop = target;
  while ((READ_OPE(obj, OP_OR, OPTS)) > 0) {
    if ((loop->next = _read_or_expression(obj)) == NULL) {
      _free_rule(target);
      return (NULL);
    }
    loop = loop->next;
    loop->next = NULL;
  }
  return (target);
}

static
tl_orexpr *
_read_couple(t_rdfl *obj, char *beg, char *end) {
  tl_orexpr	*target;

  if (rdfl_bacc_readptr(obj, beg, strlen(beg), OPTS) <= 0)
    return (NULL);
  if (!(target = _read_orloop(obj)))
    return (NULL);
  if (rdfl_bacc_readptr(obj, end, strlen(end), OPTS) <= 0) {
    _free_rule(target);
    return (NULL);
  }
  return (target);
}

// TODO rdfl_readBNF_builtins.c
static
ssize_t
read_builtin_syntax(t_rdfl *obj, void **target) {
  ssize_t		ret;
  if ((ret = READ_OPE(obj, ":", OPTS)) <= 0) return (ret);
  if ((ret = rdfl_ct_readIdentifier(obj, target, OPTS | RDFL_P_IGNORE_PREDATA)) <= 0)
    return ((!ret) ? ERRBNF_SYNTAX : ret);
  return (1);
}

tl_fact *
read_factor(t_rdfl *obj) {
  tl_fact	*fact;

  if (!(fact = malloc(sizeof(*fact))))
    return (NULL);
  fact->next = NULL;
  if (read_builtin_syntax(obj, &fact->target) > 0) {
    if (!fact->target)
      goto free_fact;
    fact->type = FACT_RULE_BUILTIN;
    return (fact);
  }
  if (rdfl_ct_readIdentifier(obj, &fact->target, OPTS) > 0) {
    if (!fact->target)
      goto free_fact;
    fact->type = FACT_RULE;
    return (fact);
  }
  // TODO builtin readString(whatever the quote)
  if (rdfl_ct_readString(obj, &fact->target, OPTS) > 0
      || rdfl_ct_readString(obj, &fact->target, OPTS | RDFL_PSTR_SIMPLE_QUOTE_STR) > 0) {
    if (!fact->target)
      goto free_fact;
    fact->type = FACT_LITERAL;
    return (fact);
  }
  if ((fact->target = (void *)_read_couple(obj, OP_EXPR_BEG, OP_EXPR_END)) != NULL) {
    int		opfeed = ((READ_OPE(obj, "+", OPTS) > 0) ? FACT_EXPR_NGT
	: (READ_OPE(obj, "*", OPTS) > 0) ? FACT_EXPR_NGTE
	: (READ_OPE(obj, "?", OPTS) > 0) ? FACT_EXPR_OPT
	: -1);
    if (opfeed == -1)
      goto free_fact;
    fact->type = opfeed;
    return (fact);
  }
free_fact:
  free(fact);
  return (NULL);
}

static
tl_orexpr *
_read_or_expression(t_rdfl *obj) {
  tl_orexpr	*expr;
  tl_fact	*fact;

  if (!(expr = malloc(sizeof(*expr))))
    return (NULL);
  expr->next = NULL;
  if (!(expr->factors = read_factor(obj))) {
    free(expr);
    return (NULL);
  }
  fact = expr->factors;
  while ((fact->next = read_factor(obj)) != NULL)
    fact = fact->next;
  return (expr);
}

static
t_rdfl_bnf *
read_production(t_rdfl *obj) {
  t_rdfl_bnf	*prod;
  e_rdflerrors	e = ERR_NONE;

  if (!(prod = malloc(sizeof(*prod))))
    return (NULL);
  prod->identifier = NULL;
  if (rdfl_ct_readIdentifier(obj, ((void *)&prod->identifier), OPTS) <= 0
      || !prod->identifier) {
    goto free_prod;
  }
  if ((!(prod->params = read_params(obj, &e))) && (e != ERR_NONE))
    goto free_prod;
  if (READ_OPE(obj, OP_RULE, OPTS) <= 0)
    goto free_prod;
  if (!(prod->exprs = _read_orloop(obj)))
    goto free_prod;
  if (READ_OPE(obj, OP_RULE_END, OPTS) <= 0)
    goto free_prod;
  prod->next = NULL;
  return (prod);
free_prod:
  free(prod->identifier);
  free(prod);
  return (NULL);
}

t_rdfl_bnf *
rdfl_readBNF(t_rdfl *obj) {
  t_rdfl_bnf	*it, *productions;

  RDFL_OPT_SET(obj->settings, RDFL_AUTOCLEAR_BLANKS);
  if (rdfl_add_comment(obj, "##", "\n") != ERR_NONE
      || rdfl_add_comment(obj, "/*", "*/") != ERR_NONE)
    return (NULL);
  if ((productions = read_production(obj)) == NULL) {
    return (NULL);
  }
  it = productions;
  while ((it->next = read_production(obj)) != NULL) {
    it = it->next;
  }
  if (!rdfl_eofreached(obj)) {
    return (NULL);
  }
  if (_link_factors(productions) == EXIT_FAILURE) {
    rdfl_freeBNF(productions);
    return (NULL);
  }
  return (productions);
}

static
void
_free_factors(tl_fact *facts) {
  tl_fact	*fact_tmp;

  while (facts) {
    fact_tmp = facts;
    facts = facts->next;
    if (fact_tmp->type >= FACT_EXPRS)
      _free_rule(((tl_orexpr *)fact_tmp->target));
    else if (fact_tmp->type == FACT_RULE || fact_tmp->type == FACT_LITERAL
	|| fact_tmp->type == FACT_RULE_BUILTIN)
      free(fact_tmp->target);
    free(fact_tmp);
  }
}

static
void
_free_rule(tl_orexpr *exprs) {
  tl_orexpr	*tmp;

  while (exprs) {
    tmp = exprs;
    exprs = exprs->next;
    _free_factors(tmp->factors);
    free(tmp);
  }
}

void
rdfl_freeBNF(t_rdfl_bnf *productions) {
  t_rdfl_bnf	*p;

  while (productions) {
    p = productions;
    productions = productions->next;
    _free_rule(p->exprs);
    _free_param_list(p->params);
    free(p->identifier);
    free(p);
  }
}
#ifdef	DEVEL

static void	_dump_rule(tl_orexpr *, unsigned int);

static
void
_dump_factors(tl_fact *facts, unsigned int level) {
  tl_fact	*first = facts;
  while (facts) {
    if (facts != first)
      printf(" ");
    if (facts->type == FACT_RULE)
      printf("<?>%s", ((char *)facts->target));
    else if (facts->type == FACT_RULE_BUILTIN)
      printf(":%s", ((char *)facts->target));
    else if (facts->type == FACT_RULE_LINKED)
      printf("%s", (((t_rdfl_bnf *)facts->target)->identifier));
    else if (facts->type == FACT_LITERAL)
      printf("%s", ((char *)facts->target));
    else if (facts->type >= FACT_EXPRS) {
      printf("[");
      _dump_rule(((tl_orexpr *)facts->target), level + 1);
      printf("]%c ", ((facts->type == FACT_EXPR_OPT) ? '?'
	    : (facts->type == FACT_EXPR_NGT) ? '+'
	    : (facts->type == FACT_EXPR_NGTE) ? '*'
	    : 'x'));
    }
    facts = facts->next;
  }
}

static
void
_dump_rule(tl_orexpr *exprs, unsigned int level) {
  int i = 0;
  while (exprs) {
    if (i) printf(" | ");
    i = 1;
    _dump_factors(exprs->factors, level);
    exprs = exprs->next;
  }
}

static
void
_dump_params(tl_param *prm) {
  printf("(%s %s", prm->type, prm->id);
  while ((prm = prm->next) != NULL) {
    printf(", %s %s", prm->type, prm->id);
  }
  printf(")");
}

void
rdfl_readBNF_dump(t_rdfl_bnf *bnf) {
  if (!bnf) {
    printf("======= No BNF\n");
    return ;
  }
  while (bnf) {
    printf("%s", bnf->identifier);
    if (bnf->params)
      _dump_params(bnf->params);
    printf(" ::= ");
    _dump_rule(bnf->exprs, 0);
    printf(";\n");
    bnf = bnf->next;
  }
}
#endif
