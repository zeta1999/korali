import numpy as np

def univariate_gaussian_probability(mu, sigma:float, point):
    point = np.array(point)
    mu = np.array(mu)
    n = len(mu)
    assert (len(mu) == len(point));
    squared_distance = np.inner(mu - point, mu - point)
    denominator = (np.sqrt(2.0 * np.pi) * sigma) ** n
    probability = np.exp(- squared_distance / (2.0 * sigma ** 2)) / denominator
    return probability



def generate_variable(transf, experiment, idx, name, distribs, initial=None):
    ''':param experiment: korali-experiment
        :param idx: The variable index of the variable to create
        :distribs: A dict mapping distribution types to prior distribution names'''
    if transf == 0:  # Normal
        experiment["Variables"][idx]["Name"] = "(Normal) "+name
        experiment["Variables"][idx]["Initial Value"] = -5. if initial is None else initial
        experiment["Variables"][idx]["Bayesian Type"] = "Latent"
        experiment["Variables"][idx]["Latent Variable Distribution Type"] = "Normal"
        experiment["Variables"][idx]["Prior Distribution"] = distribs["Normal"]  # not used, but required
    elif transf == 1:  # lognormal
        experiment["Variables"][idx]["Name"] = "(Log-normal) "+name
        experiment["Variables"][idx]["Initial Value"] = 5. if initial is None else initial
        experiment["Variables"][idx]["Bayesian Type"] = "Latent"
        experiment["Variables"][idx]["Latent Variable Distribution Type"] = "Log-Normal"
        experiment["Variables"][idx]["Prior Distribution"] = distribs["Log-Normal"] # not used, but required
    elif transf == 2:  # probit-normal
        raise NotImplementedError("Probit-normal variables not yet implemented")
        experiment["Variables"][idx]["Name"] = "(Probit-normal) "+name
        experiment["Variables"][idx]["Initial Value"] = 999999. if initial is None else initial
        experiment["Variables"][idx]["Bayesian Type"] = "Latent"
        experiment["Variables"][idx]["Latent Variable Distribution Type"] = "Probit-Normal"
        experiment["Variables"][idx]["Prior Distribution"] = distribs["Probit-Normal"] # not used, but required
    elif transf == 3:  # logit-normal
        experiment["Variables"][idx]["Name"] = "(Logit-normal) "+name
        experiment["Variables"][idx]["Initial Value"] = 0.5 if initial is None else initial
        experiment["Variables"][idx]["Bayesian Type"] = "Latent"
        experiment["Variables"][idx]["Latent Variable Distribution Type"] = "Logit-Normal"
        experiment["Variables"][idx]["Prior Distribution"] = distribs["Logit-Normal"] # not used, but required
    else:
        raise ValueError("Only values 0 - 3 code for latent variable types")
    return None




