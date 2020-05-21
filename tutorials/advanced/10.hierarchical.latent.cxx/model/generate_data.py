import numpy as np


#
#  Model 3:
#    draw psi_i ~ N(theta, omega**2)
#    draw x_i ~ N(psi_i, sigma**2)
#


def draw_from_simple_hierarchical(n_individuals, sigma, omega, mean, n_samples=1):
    ''' '''
    results = np.zeros((n_individuals, n_samples))
    latent_vars = np.random.normal(mean, scale=omega, size=(n_individuals,))
    for i in range(n_individuals):
        lvar = latent_vars[i]
        results[i, :] = np.random.normal(lvar, scale=sigma, size=(n_samples,))
    return results

# def draw_from_multiple_gaussian(nr_points=10, dimensions=2, mus=[[0, 0],[2,2]], sigma=1):
#     results = np.zeros((nr_points, dimensions))
#     cluster_assignment = np.zeros((nr_points,), dtype=int)
#     nr_clusters = len(mus)
#     for i in range(nr_points):
#         c = np.random.choice(list(range(nr_clusters)))
#         mu = mus[c]
#         pt = np.random.normal(loc=mu, scale=sigma)
#         cluster_assignment[i] = c
#         results[i, :] = pt
#     return results, cluster_assignment


def generate_data():
    n_individuals =   10
    n_samples = 1   # how many datapoints per individual
    sigma = .5
    omega = 2.
    mean = 5 # The Hyperparameter
    output_file_925 = "data_925.in" # 925 for section 9.2.5

    simple_data = draw_from_simple_hierarchical(n_individuals, sigma=sigma, omega=omega, mean=mean, n_samples=n_samples)

    # store to files:
    with open(output_file_925, "w") as fd:
        d = simple_data
        fd.write(str(n_individuals) +" " + str(n_samples) + " " + str(sigma) + " " + str(omega)  +"\n")
        lines = [" ".join([str(d[i, j]) for j in range(n_samples)])  + "\n"   for i in range(n_individuals)]
        fd.writelines(lines)

    print("Done")


if __name__ == '__main__':
    generate_data()