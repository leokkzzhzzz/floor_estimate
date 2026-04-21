from setuptools import find_packages
from setuptools import setup

setup(
    name='barometer_interfaces',
    version='0.0.0',
    packages=find_packages(
        include=('barometer_interfaces', 'barometer_interfaces.*')),
)
