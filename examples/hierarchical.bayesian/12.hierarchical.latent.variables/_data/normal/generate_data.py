import numpy as np

import sys, os
scriptdir = os.path.dirname(os.path.abspath(__file__))
# sys.path.append('./_model/normal')
sys.path.append(os.path.join(scriptdir, '../../_model/normal'))
from model import *

def generate_normal_data(N=None, x=None):
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
        N = 200 # ?
    Nx = 1
    if x is None:
        x =  np.array([0 for _ in range(N)])

    y = np.zeros((N, Nx))
    theta = np.zeros((N, 2))
    mean_1 = 5
    sdev_1 = 0.1
    fixed_val_2 = 1

    for i in range(N):
        theta[i, 0] = np.random.normal(mean_1, sdev_1)
        theta[i, 1] = fixed_val_2

        y[i,:] = normalModel(x[i], theta[i, 0] )
        error =  np.random.normal(0, theta[i, 1], size=(1, Nx))
        y[i,:] = y[i,:] + error

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

    with open("all_data_info.txt", "w") as fd:
        fd.write( f'N: {N}\t Mean: [{mean_1}, {fixed_val_2}] (2nd is the noise sdev in the model)\n')
        fd.write( f'True covariance: [[{sdev_1**2:.4f}, \t0.0],\n')
        fd.write( f'                  [0.0, \t0.0]]\n')
        fd.write("## True sampled latent variable vectors:\n")
        for i, th in enumerate(theta):
            fd.write( f"Indiv. {i}: \t["+ ", ".join([str(t) for t in th]) + "]\n")


    return y, theta, FLAG


if __name__ == '__main__':
    generate_normal_data()