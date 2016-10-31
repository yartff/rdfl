#include		<stdlib.h>
#include		<string.h>
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"
#include		"rdfl_readBNF.h"

#define		OP_RULE			"::="
#define		OP_RULE_END		";"
#define		OP_OR			"|"
#define		OP_OPT_BEG		"["
#define		OP_OPT_END		"]"
#define		OP_REP_BEG		"{"
#define		OP_REP_END		"}"
#define		OP_GRP_BEG		"("
#define		OP_GRP_END		")"
#define		READ_OPE(obj, c, opt)	(rdfl_bacc_readptr(obj, c, sizeof(c) - 1, opt) > 0)
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
    else if (facts->type >= FACT_EXPR_OPT) {
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

static const e_bacc_options	OPTS = RDFL_P_NULLTERMINATED | RDFL_P_CONSUME;
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

tl_fact *
read_factor(t_rdfl *obj) {
  tl_fact	*fact;

  if (!(fact = malloc(sizeof(*fact))))
    return (NULL);
  fact->next = NULL;
  if (rdfl_ct_readIdentifier(obj, &fact->target, OPTS) > 0) {
    if (!fact->target)
      goto free_fact;
    fact->type = FACT_RULE;
    return (fact);
  }
  if (rdfl_ct_readString(obj, &fact->target, OPTS) > 0
      || rdfl_ct_readString(obj, &fact->target, OPTS | RDFL_PSTR_SIMPLE_QUOTE_STR) > 0) {
    if (!fact->target)
      goto free_fact;
    fact->type = FACT_LITERAL;
    return (fact);
  }
  if ((fact->target = (void *)_read_couple(obj, OP_OPT_BEG, OP_OPT_END)) != NULL) {
    fact->type = FACT_EXPR_OPT;
    return (fact);
  }
  if ((fact->target = (void *)_read_couple(obj, OP_REP_BEG, OP_REP_END)) != NULL) {
    fact->type = FACT_EXPR_REP;
    return (fact);
  }
  if ((fact->target = (void *)_read_couple(obj, OP_GRP_BEG, OP_GRP_END)) != NULL) {
    fact->type = FACT_EXPR_GROUP;
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

  if (!(prod = malloc(sizeof(*prod))))
    return (NULL);
  prod->identifier = NULL;
  if (rdfl_ct_readIdentifier(obj, ((void *)&prod->identifier), OPTS) <= 0
      || !prod->identifier) {
    goto free_prod;
  }
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
  if (rdfl_set_comment(obj, "##", "\n") != ERR_NONE
      || rdfl_set_comment(obj, "/*", "*/") != ERR_NONE)
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
    if (fact_tmp->type == FACT_RULE || fact_tmp->type == FACT_LITERAL)
      free(fact_tmp->target);
    else if (fact_tmp->type >= FACT_EXPR_OPT)
      _free_rule(((tl_orexpr *)fact_tmp->target));
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
    free(p->identifier);
    free(p);
  }
}

#ifdef	DEVEL

static void	_dump_rule(tl_orexpr *, unsigned int, int);
static
void
_dump_level(unsigned int level) {
  while (level) {
    printf(" -");
    --level;
  }
  printf("> ");
}

static
void
_dump_factors(tl_fact *facts, unsigned int level) {
  while (facts) {
    if (facts->type == FACT_RULE)
      printf("+%s ", ((char *)facts->target));
    else if (facts->type == FACT_RULE_LINKED)
      printf("+%s ", (((t_rdfl_bnf *)facts->target)->identifier));
    else if (facts->type == FACT_LITERAL)
      printf("_%s ", ((char *)facts->target));
    else {
      if (facts->type == FACT_EXPR_OPT)
	printf("[]<");
      if (facts->type == FACT_EXPR_REP)
	printf("{}<");
      if (facts->type == FACT_EXPR_GROUP)
	printf("()<");
      _dump_rule(((tl_orexpr *)facts->target), level + 1, 0);
      printf("> ");
    }
    facts = facts->next;
  }
}

static
void
_dump_rule(tl_orexpr *exprs, unsigned int level, int pnl) {
  int i = 0;
  while (exprs) {
    if (pnl == 1)
      _dump_level(level);
    else if (i) {
      printf("| ");
    }
    i = 1;
    _dump_factors(exprs->factors, level);
    if (pnl == 1)
      printf("\n");
    exprs = exprs->next;
  }
}

void
rdfl_readBNF_dump(t_rdfl_bnf *bnf) {
  if (!bnf) {
    printf("======= No BNF\n");
    return ;
  }
  while (bnf) {
    printf("\n=======\nRule: %s\n", bnf->identifier);
    _dump_rule(bnf->exprs, 0, 1);
    bnf = bnf->next;
  }
}
#endif
