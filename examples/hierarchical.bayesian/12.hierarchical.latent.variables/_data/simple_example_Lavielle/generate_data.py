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


def generate_data():
    n_individuals =   10
    n_samples = 1   # how many datapoints per individual
    sigma = .5
    omega = 2.
    mean = 1 # The Hyperparameter
    output_file_925 = "data_925.in"  # 925 for section 9.2.5
    info_output_file_925 = "data_925_info.txt"

    simple_data = draw_from_simple_hierarchical(n_individuals, sigma=sigma, omega=omega, mean=mean, n_samples=n_samples)

    # store to files:
    with open(output_file_925, "w") as fd:
        d = simple_data
        fd.write(str(n_individuals) +" " + str(n_samples) + " " + str(sigma) + " " + str(omega)  +"\n")
        lines = [" ".join([str(d[i, j]) for j in range(n_samples)])  + "\n"   for i in range(n_individuals)]
        fd.writelines(lines)

    # calculate the optimizer (just the mean) and store it as reference
    with open(info_output_file_925, "w") as fd:
        fd.write("# The optimizing hyperparameter (the mean):"+"\n")
        optimizer = np.mean(simple_data)
        fd.write("# \t%.3f\n" % optimizer)
        fd.write("# Original mean (used to create data):\n")
        fd.write("# \t"+str(mean)+"\n")
        fd.write("# Original hyperparameter variance:\n# \tw^2 with w == "+str(omega)+"\n")
    print("Done generating data according to section 9.2.5 in 'Mixed Effect Models' by M. Lavielle.")



if __name__ == '__main__':
    generate_data()