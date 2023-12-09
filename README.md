# CARM Electronics

This document provides instructions on setting up the Conda environment for the project using the provided ```carm.yml``` file.

## Prerequisites

- [Conda](https://docs.conda.io/en/latest/) or [Miniconda](https://docs.conda.io/projects/miniconda/en/latest/)

## Getting Started

Follow the steps below to set up the Conda environment for the project:

```bash
# 1. Clone the Repository
git clone https://github.com/bimmui/CARM-electronics.git
cd CARM-electronics

# 2. Create Conda Environment
conda env create -f carm.yml -n carm

# 3. Activate the Conda Environment
conda activate carm

# 4. Verify Installation
conda list
