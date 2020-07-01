
import numpy as np


def extr(line, toint=True, tofloat=False, delim=" "):
    result =  line.strip("\n ").split(delim)
    if tofloat:
        result = [float(r) for r in result]
    elif toint:
        result = [int(r) for r in result]
    return result


class NormalData():
    def __init__(self):
        self.nIndividuals = None
        self.nDataTotal = None

        self.error = "ind" # no other choice
        self.error_model = "constant" # might add "proportional" option

        self.nSamplesEach = []
        self.data = []
        filename = '../../_data/normal/all_data.txt'
        delimiterIn = '\t'
        print(f"Loading data from {filename} ... \n")
        with open(filename, "r") as fd:
            self.column_names = fd.readline().strip("\n ").split(delimiterIn)
            lines = fd.readlines()
            data = [extr(line, tofloat=True, delim=delimiterIn) for line in lines]
            data = np.array(data)
            uid = np.unique(data[:, 0])
            self.nIndividuals = len(uid)
            self.nDataTotal = len(data)
            self.nSamplesEach = np.zeros((self.nIndividuals))
            for i in range(self.nIndividuals):
                self.nSamplesEach[i] = np.sum(data[:, 0] == i)
            assert np.sum(self.nSamplesEach) == len(data)

        self.beta = 2
        self.omega = [[1**2]]
        self.alpha = 1
        self.Nmp = self.N = 1
        self.transf = 0
        self.omega_chol = np.linalg.cholesky(self.omega)
        self.sigma = 1 * np.eye(self.N)

        self.data = data

    #
    # def reset_to(self, nIndividuals, sigma, omega, data, nSamplesEach=1):
    #     self.data = np.array(data).flatten()
    #     self.nIndividuals = nIndividuals
    #     self.sigma = sigma
    #     self.omega = omega
    #     self.nSamplesEach = nSamplesEach
    #     assert len(data) == nIndividuals
