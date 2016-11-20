#include	<stdio.h>
#include	"rdfl_bnf.h"

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
