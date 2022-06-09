from setuptools import setup, find_packages
from distutils.core import setup

setup(
    name='rentex',
    version='0.1',
    packages=find_packages(),
    install_requires=[
        "attrs==21.4.0",
        "cffi==1.15.0",
        "click==8.0.3",
        "colorama==0.4.4",
        "cryptography==36.0.1",
        "Deprecated==1.2.13",
        "Flask==2.0.2",
        "Flask-Cors==3.0.10",
        "Flask-JWT-Extended==4.3.1",
        "greenlet==1.1.2",
        "itsdangerous==2.0.1",
        "Jinja2==3.0.3",
        "jsonschema==4.4.0",
        "MarkupSafe==2.0.1",
        "packaging==21.3",
        "psycopg2==2.9.3",
        "pycparser==2.21",
        "PyJWT==2.3.0",
        "pyparsing==3.0.7",
        "pyrsistent==0.18.1",
        "python-dotenv==0.19.2",
        "redis==4.1.2",
        "six==1.16.0",
        "SQLAlchemy==1.4.31",
        "Werkzeug==2.0.2",
        "wrapt==1.13.3",
    ],
    include_package_data=True
)
