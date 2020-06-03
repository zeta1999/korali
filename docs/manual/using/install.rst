*********************
Installation
*********************

Installation Steps
====================

1. Download Korali

  Download the latest stable release of Korali with the following command:
  
  .. code-block:: bash
    
     git clone https://github.com/cselab/korali.git --recursive --branch latest

2. Install Korali

  To build and install Korali, simply run:

  .. code-block:: bash
 
     cd korali
     ./install

  For a faster installation, you can build Korali with parallel jobs. E.g.,

  .. code-block:: bash
   
     ./install --jobs=8

  If you are missing any prerequsites, you can ask Korali to install them automatically via:

  .. code-block:: bash
   
     ./install --prereqs

Troubleshooting
====================

If you are experiencing problems installing or running Korali, please check the following hints:

  - Check Korali's `system requirements <#system-requirements>`_ to verify that your system has all the required software components.

  - Check the `build status </korali/docs/dev/testing.html>`_  to see if Korali is currently building correctly.

  - If the problem persists, please submit a new `issue report <https://github.com/cselab/korali/issues>`_ on our Github repository detailing the problem, your system information, and the steps to replicate it and we will promptly address it.

  - For further questions, feel free to `contact us </korali/#contact>`_.


Cray systems (Piz Daint)
------------------------

The default installation of `mpi4py` possibly uses a different MPI implementation than Korali, preventing multi-rank runs.
To fix it, configure MPI compilers and reinstall `mpi4py` and Korali.

.. code-block:: bash

    # Create wrappers `mpicc` and `mpic++` around Cray compilers `cc` and `CC`, respectively.
    # Warning: this will overwrite any `mpicc` and `mpic++` in your `~/bin` folder!
    mkdir -p $HOME/bin
    echo -e '#!/bin/bash'"\n"'cc "$@"' > $HOME/bin/mpicc
    echo -e '#!/bin/bash'"\n"'CC "$@"' > $HOME/bin/mpic++
    chmod +x $HOME/bin/mpicc $HOME/bin/mpic++

    # Load Python module (you can add this to your `~/.bashrc`).
    module load cray-python
    
    # Reinstall mpi4py locally and reinstall korali.
    python3 -m pip install --user mpi4py --ignore-installed -v
    cd ~/path/to/korali
    MPICXX=mpic++ ./install --rebuild --jobs=12

System Requirements
====================

Mandatory Requirements
---------------------------

  - **C++ Compiler**
      Korali requires a C++ compiler that supports the C++14 standard (`-std=c++14`) to build.
      **Hint:** Check the following `link <https://en.cppreference.com/w/cpp/compiler_support#C.2B.2B14_core_language_features>`_ to verify whether your compiler supports C++14.
      Korali's installer will check the **$CXX** environment variable to determine the default C++ compiler. You can change the value of this variable to define a custom C++ compiler.
  
  - **CMake**
      Korali requires that you have `CMake <https://cmake.org/>`_ version 3.0 or higher installed in your system.  If CMake is not found, you can ask Korali to install it automatically by using the ``--prereqs`` option.
      
  - **wget**
      Korali requires access to the internet and the *wget* command to be available to automatically resolve some of its dependencies. If you wish to install Korali on a system without access to internet or no *wget* command, you check the `Installed by Korali <#automatically-installed-by-korali>`_ section of this document to manually define these requirements.
  
  - **Git Client**
      You need Git to clone (download) our code before installation.
  
  - **Python3**
      Korali requires a version of Python higher than 3.0 to be installed in the system. Korali's installer will check the *python3* command. The path to this command should be present in the $PATH environment variable. *Hint:* Make sure Python3 is correctly installed or its module loaded before configuring Korali.
  
  - **python3-config**
      Korali requires the command *python3-config* to be available during installation. This command is typically included in standard installations of python3 that include developer tools. *Hint:*  If *python3-config* is missing, you can get it by installing/loading the **python3-dev** package/module in your system.
  
  - **Pip3 Installer**
      Korali requires the *pip3* command to install it's engine and tools. This command is typically included in standard installations of python. *Hint:*  If *pip3* is missing, you can get it by installing pip3, with e.g, ``brew install pip3``
  
  - **PyBind11**
      Korali requires *pybind11* to enable Python/C++ interaction. If not found, it will try to install it automatically using *pip3*.
  
  - **GNU Scientific Library**
      Korali requires that the `GSL-2.4 <http://www.gnu.org/software/gsl/>`_ or later must be installed on your system. If the command ``gsl-config`` is not found, you can ask Korali  to install GSL automatically by using the ``--prereqs`` option. 

  - **oneDNN**
      Korali requires that the `OneAPI Deep Neural Network Library <https://oneapi-src.github.io/oneDNN/>`_ or later must be installed on your system. If you have an installation of oneDNN already in your system, make sure the environment variable ``DNNLROOT`` pointing to its installation folder is correctly defined. If OneDNN is not found, you can ask Korali to install it automatically  by using the ``--prereqs`` option.

Optional Requirements
---------------------------------

  - **MPI Library**
      One way to enable support distributed conduits and computational models is to configure Korali to compile with an MPI compiler. The installer will check the *$MPICXX* environment variable to determine a valid MPI C++ compiler.

