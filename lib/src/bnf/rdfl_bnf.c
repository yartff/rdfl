#include		<stdlib.h>
#include		<string.h>
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"
#include		"rdfl_bnf.h"
#include		"rdfl_local.h"

static int		_link_exprs(t_rdfl_bnf *tmp, tl_orexpr *);
tl_fact			*_read_factor(t_rdfl *, e_rdflerrors *);
static void		_free_rule(tl_orexpr *);

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

// vvvvvvv up to this for parsing
static
tl_orexpr *
_read_or_expression(t_rdfl *obj, e_rdflerrors *e) {
  tl_orexpr	*expr;
  tl_fact	*fact;

  if (!(expr = malloc(sizeof(*expr))))
    return (NULL);
  expr->next = NULL;
  if (!(expr->factors = _read_factor(obj, e))) {
    free(expr);
    return (NULL);
  }
  fact = expr->factors;
  while ((fact->next = _read_factor(obj, e)) != NULL)
    fact = fact->next;
  return (expr);
}

static
tl_orexpr *
_read_orloop(t_rdfl *obj, e_rdflerrors *e) {
  tl_orexpr	*target, *loop;

  if ((target = _read_or_expression(obj, e)) == NULL)
    return (NULL);
  target->next = NULL;
  loop = target;
  while ((READ_OPE(obj, OP_OR, OPTS)) > 0) {
    if ((loop->next = _read_or_expression(obj, e)) == NULL) {
      *e = ERRBNF_SYNTAX;
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
_read_couple(t_rdfl *obj, char *beg, char *end, e_rdflerrors *e) {
  tl_orexpr	*target;

  if (rdfl_bacc_readptr(obj, beg, strlen(beg), OPTS) <= 0)
    return (NULL);
  if (!(target = _read_orloop(obj, e)))
    return (NULL);
  if (rdfl_bacc_readptr(obj, end, strlen(end), OPTS) <= 0) {
    _free_rule(target);
    return (NULL);
  }
  return (target);
}

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
_read_factor(t_rdfl *obj, e_rdflerrors *e) {
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
  if ((fact->target = (void *)_read_couple(obj, OP_EXPR_BEG, OP_EXPR_END, e)) != NULL) {
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

// vvv DONE
//
static
t_rdfl_bnf *
read_production(t_rdfl *obj, e_rdflerrors *e) {
  t_rdfl_bnf	*prod;
  ssize_t	ret;
  char		*cpy_identifier = NULL;
  tl_param	*cpy_params = NULL;
  tl_orexpr	*cpy_exprs = NULL;

  if ((ret = rdfl_ct_readIdentifier(obj, ((void *)&cpy_identifier), OPTS)) <= 0)
    goto free_prod_customerr;
  if (!(cpy_params = read_params(obj, e)) && (*e != ERR_NONE))
    goto free_prod;
  if ((ret = READ_OPE(obj, OP_RULE, OPTS)) <= 0)
    goto free_prod_customerr;
  if (!(cpy_exprs = _read_orloop(obj, e)) && *e != ERR_NONE)
    goto free_prod;
  if ((ret = READ_OPE(obj, OP_RULE_END, OPTS)) <= 0)
    goto free_prod_customerr;
  if (!(prod = malloc(sizeof(*prod)))) {
    *e = ERR_MEMORY;
    goto free_prod;
  }

  prod->identifier = cpy_identifier;
  prod->params = cpy_params;
  prod->exprs = cpy_exprs;
  prod->next = NULL;
  return (prod);
free_prod_customerr:
  if (ret != ERR_NONE)
    *e = ret;
free_prod:
  free(cpy_identifier);
  _free_param_list(cpy_params);
  _free_rule(cpy_exprs);
  return (NULL);
}

// Logic:
// if returns NULL, e IS set (ERR_NONE or ERR_*)
// if returns not NULL, e == ERR_NONE

static
t_rdfl_bnf *
read_prodloop(t_rdfl *obj, e_rdflerrors *e) {
  t_rdfl_bnf	*it, *productions;

  if ((productions = read_production(obj, e)) == NULL)
    return (NULL);
  it = productions;
  while ((it->next = read_production(obj, e)) != NULL)
    it = it->next;
  if (*e != ERR_NONE || !rdfl_eofreached(obj)) {
    rdfl_freeBNF(productions);
    return (NULL);
  }
  return (productions);
}

t_rdfl_bnf *
rdfl_readBNF(t_rdfl *obj) {
  t_rdfl_bnf	*productions;
  e_rdflerrors	e = ERR_NONE;

  RDFL_OPT_SET(obj->settings, RDFL_AUTOCLEAR_BLANKS);
  if (rdfl_add_comment(obj, "##", "\n") != ERR_NONE
      || rdfl_add_comment(obj, "/*", "*/") != ERR_NONE)
    return (NULL);
  if (!(productions = read_prodloop(obj, &e))) {
    // Handle err
    fprintf(stderr, "RET[%d]\n", e);
    return (NULL);
  }
  if (_link_factors(productions) == EXIT_FAILURE) {
    rdfl_freeBNF(productions);
    return (NULL);
  }
  return (productions);
}

//
// TODO check *e ^^^^^^^^^^^^^

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
