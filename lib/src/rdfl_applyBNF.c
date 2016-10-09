#include		<stdlib.h>
#include		<string.h>
#include		"rdfl_buffer_access.h"
#include		"rdfl_consumer.h"
#include		"rdfl_context.h"
#include		"rdfl_readBNF.h"

static const e_bacc_options	OPTS = RDFL_P_NULLTERMINATED;
static ssize_t			_eval_orexpr(tl_orexpr *, t_rdfl_bnf *, t_rdfl *);
static ssize_t			_eval_rule(t_rdfl_bnf *, t_rdfl_bnf *, t_rdfl *);

// TODO wipe consume
static
ssize_t
_eval_expr_literal(char *ptr, t_rdfl *fl) {
  // fprintf(stdout, "Literal:{%s}\n", ptr);
  return (rdfl_bacc_readptr(fl, ptr + 1, strlen(ptr) - 2, OPTS));
}

static
ssize_t
_eval_expr_opt(tl_orexpr *expr, t_rdfl_bnf *bnf, t_rdfl *fl) {
  ssize_t	ret = _eval_orexpr(expr, bnf, fl);
  if (ret < 0)
    return (ret);
  return (1);
}

static
ssize_t
_eval_expr_rep(tl_orexpr *expr, t_rdfl_bnf *bnf, t_rdfl *fl) {
  ssize_t	ret;
  while ((ret = _eval_orexpr(expr, bnf, fl)) >= 0) {
    if (ret == 0)
      return (1);
  }
  return (ret);
}

static
ssize_t
_eval_expr_group(tl_orexpr *expr, t_rdfl_bnf *bnf, t_rdfl *fl) {
  // ssize_t	ret;
  // TODO use context | RDFL_P_SETCONTEXT
  return (_eval_orexpr(expr, bnf, fl));
}

// TODO #ifdef DEVEL
static int level = 0;

// static
void dump_fact(tl_fact *fact) {
  int l = level;
  while (l) {
    fprintf(stdout, "  ");
    --l;
  }
  fprintf(stdout, "%%");
  if (fact->type == FACT_RULE_LINKED)
    fprintf(stdout, "+%s\n", (((t_rdfl_bnf *)fact->target)->identifier));
  else if (fact->type == FACT_LITERAL)
    fprintf(stdout, "+%s\n", ((char *)fact->target));
}

// TODO
# include	"rdfl_devel.h"

static
ssize_t
_eval_expr(tl_fact *fact, t_rdfl_bnf *bnf, t_rdfl *fl) {
  ssize_t	ret = 0;
  ssize_t	ctx_id;

  ctx_id = rdfl_pushcontext(fl);
  (void)ctx_id;
  while (fact) {
    rdfl_restaurecontext(fl, -1);
    rdfl_printbufferstate(fl);
    // dump_fact(fact);
    ret = ((fact->type == FACT_RULE_LINKED) ? _eval_rule(((t_rdfl_bnf *)fact->target), bnf, fl)
	: (fact->type == FACT_LITERAL) ? _eval_expr_literal(((char *)fact->target), fl)
	: (fact->type == FACT_EXPR_OPT) ? _eval_expr_opt(((tl_orexpr *)fact->target), bnf, fl)
	: (fact->type == FACT_EXPR_REP) ? _eval_expr_rep(((tl_orexpr *)fact->target), bnf, fl)
	: (fact->type == FACT_EXPR_GROUP) ? _eval_expr_group(((tl_orexpr *)fact->target), bnf, fl)
	: 0);
    // rdfl_printbufferstate(fl);
    if (ret > 0) {
      fprintf(stdout, "<<<========%zd TYPE %d\n", ret, fact->type);
      rdfl_printbufferstate(fl);
    }
    if (ret <= 0) {
      // rdfl_restaurecontext(fl, ctx_id);
      return (ret);
    }
    fact = fact->next;
  }
  return (ret);
}

static
ssize_t
_eval_orexpr(tl_orexpr *or, t_rdfl_bnf *bnf, t_rdfl *fl) {
  ssize_t	ret;
  // TODO #ifdef DEVEL
  ++level;
  if ((ret = _eval_expr(or->factors, bnf, fl)) < 0)
    return (ret);
  while (ret == 0 && ((or = or->next) != NULL)) {
    if ((ret = _eval_expr(or->factors, bnf, fl)) < 0)
      return (ret);
  }
  --level;
  return (ret);
}

static
ssize_t
_eval_rule(t_rdfl_bnf *rule, t_rdfl_bnf *bnf, t_rdfl *fl) {
  fprintf(stderr, "++++++calling: %s\n", rule->identifier);
  return (_eval_orexpr(rule->exprs, bnf, fl));
}

ssize_t
rdfl_applyBNF(t_rdfl_bnf *bnf, t_rdfl *fl, char *id) {
  t_rdfl_bnf	*rule;
  // init tree in fl
  if (!bnf)
    return (ERR_MISC);
  if (!(rule = _seek_target(bnf, id)))
    return (BNF_MISSINGID);
  return (_eval_rule(rule, bnf, fl));
}