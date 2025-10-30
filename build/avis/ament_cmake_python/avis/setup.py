from setuptools import find_packages
from setuptools import setup

setup(
    name='avis',
    version='0.0.0',
    packages=find_packages(
        include=('avis', 'avis.*')),
)
