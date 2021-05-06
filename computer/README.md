PPI (Peria-pi)
==============

PPI stands for "Peria-PI" or "Peta digits of PI", and it will be able to
compute >=10^18 digits of pi in borg system.


# Usage

```
$ ./ppi 100000
```
Launching `ppi` with a numerical argument computes the number of digits of pi.
The output will be in file(s).  Some statistics are also logged into a file.


## Outputs

- Pi in decimal format (maybe in multiple files)
- Pi in hexadecimal format (maybe in multiple files)
- Statistics
- Working files
  - Configurations (# of threads, kind of parallelization .etc)
  - Past statistics
  - Milestone dumps in binary (maybe in multiple files)


# Options

- # of decimal digits: Required unless resuming
- kind of constant: Default to pi.
- formula: Choosable if a const. has multiple formulae.
- # of threads per process: Default to # of cores
- resuming a suspended work: Default to yes
- use borg: Default to no
