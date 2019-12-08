// Boundaries
// Return Values
// DEVEL


typedef struct	{
  int		type; // file or pub
  const char	*name;
  const char	**categories;
  struct {
    int		active;
    int		tested;
    // struct results
  }		runtime;
}		t_test;
