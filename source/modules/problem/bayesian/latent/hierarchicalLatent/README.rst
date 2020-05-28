**************************
HierarchicalLatent
**************************

Hierarchical latent problems impose a specific hierarchical
form on the total likelihood:

 :math:`p( d, \theta  | \psi ) = \left(\prod_{i=0}^N \prod_{j=0}^{n_i} p(d_{i,j} | \theta_i) \right) \cdot p(\theta_i | \psi)`


where

- :math:`d` is the data, where we have a varying number :math:`n_i` of data points :math:`d_{i,j}` for each 'individual' :math:`i`
- Vectors :math:`\theta_i` are latent variables, one per 'individual' :math:`i`
- :math:`\psi` are a number of hyperparameters.


Please refer to the corresponding tutorial / example for further explanation, such
as the form that we impose on :math:`p(\theta | \psi)`.