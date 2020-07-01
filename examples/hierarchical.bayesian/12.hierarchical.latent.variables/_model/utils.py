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




