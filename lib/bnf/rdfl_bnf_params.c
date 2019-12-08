#include		<stdlib.h>
#include		<string.h>
#include		"buffer_access.h"
#include		"rdfl_consumer.h"
#include		"bnf.h"
#include		"values.h"

void
_free_param_list(tl_param *p) {
  tl_param	*c = p;
  while (p) {
    c = p;
    p = p->next;
    free(c->type);
    free(c->id);
    free(c);
  }
}

static
tl_param *
read_param_each(t_rdfl *obj, e_rdflerrors *e) {
  void		*data_type = NULL, *data_id;
  tl_param	*new = NULL;

  if ((rdfl_csm_readIdentifier(obj, &data_type, OPTS) <= 0 || !data_type)
      || (rdfl_csm_readIdentifier(obj, &data_id, OPTS) <= 0 || !data_id)) {
    free(data_type);
    if (data_type )// || value of rdfl_csm_readIdentifier > 0)
    { *e = ERRBNF_SYNTAX; return (NULL); }
    return (NULL);
  }
  if (!(new = calloc(1, sizeof(*new)))) {
    free(data_type);
    free(data_id);
    *e = ERR_MEMORY_ALLOC;
    return (NULL);
  }
  new->type = (char *)data_type;
  new->id = (char *)data_id;
  new->next = NULL;
  return (new);
}

static
tl_param *
read_param_list(t_rdfl *obj, e_rdflerrors *e) {
  tl_param	*list, *new;

  if (!(list = read_param_each(obj, e)))
    return (NULL);
  new = list;
  while (READ_OPE(obj, ",", OPTS) > 0) {
    if ((new->next = read_param_each(obj, e)) == NULL) {
      if (*e == ERR_NONE)
	*e = ERRBNF_SYNTAX;
      _free_param_list(list);
      return (NULL);
    }
    new = new->next;
  }
  return (list);
}

tl_param *
read_params(t_rdfl *obj, e_rdflerrors *e) {
  tl_param	*list;

  if (READ_OPE(obj, "(", OPTS) <= 0)
    return (NULL);
  if (!(list = read_param_list(obj, e)) && *e != ERR_NONE)
    return (NULL);
  if (READ_OPE(obj, ")", OPTS) <= 0) {
    _free_param_list(list);
    return (NULL);
  }
  return (list);
}
