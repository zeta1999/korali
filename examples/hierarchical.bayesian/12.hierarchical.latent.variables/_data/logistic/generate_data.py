import numpy as np

import sys, os
scriptdir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(scriptdir, '../../_model/logistic'))
from model import *

def generate_logistic_data(N=None, x=None):
    '''
    :param N: Num individuals
    :param x: list of time points
    :returns: y: The observed values at each time point in x
              theta: Latent variable vectors, one for each individual
              FLAG: 1, to indicate that ...
    '''
    folder_name = './'


    FLAG = 1
    if N is None:
        N = 10 # number individuals
    if x is None:
        x =  np.array([np.arange(0,10.1,0.5) for _ in range(N)])


    Nx = x.shape[1]

    y = np.zeros((N, Nx))
    theta = np.zeros((N, 4))

    for i in range(N):

        theta[i, 0] = np.random.normal(200, 20)
        theta[i, 1] = np.random.normal(40, 10)
        theta[i, 2] = np.random.normal(1, 0.1)
        theta[i, 3] = 5

        y[i,:] = logisticModel(x[i,:], theta[i, :3] )
        error =  np.random.normal(0, theta[i, 3], size=(1, Nx))
        y[i,:] = y[i,:] + error

        # df =
        # data.x = x[i,:]
        # data.y = y[i,:]
        # data.theta = theta[i, 0:3]
        # data.std_data = theta[i, 3]

        file_name = f'{folder_name}data_set_{i:3d}.mat'
        # with open(file_name, 'w') as fd:
        #     fd.writelines([f'{x_} {y_} {}'])
        # save(file_name, 'data')

    all_data_ids = np.array([np.arange(N) for _ in range(Nx)]).flatten()[:, np.newaxis]
    all_data_x = x.flatten()[:, np.newaxis]
    all_data_y = y.flatten()[:, np.newaxis]
    assert len(all_data_ids) == len(all_data_x) == len(all_data_y)
    with open('all_data.txt', 'w') as fd:
        fd.write( '%6s \t %6s \t %s\n' % ("ID", "time", "y"))
        fd.writelines([ '%f \t %f \t %f\n' % (id, x_, y_) for id, x_, y_ in zip(all_data_ids, all_data_x, all_data_y)])

    return y, theta, FLAG


if __name__ == '__main__':
    generate_logistic_data()