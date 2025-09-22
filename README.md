# ortrun

Only C depended command runner with env file.

## Setup

Download this project.

```bash
git clone https://github.com/saracalihan/ortrun.git
cd ortrun
```

Compile the code.

```bash
make
```

Ready to use

## Usage

Help command:

```bash
ortrun -h
```

```bash
ortrun -f test/.env bash test/test.sh
```

`-f` is optional flag that define env file path. On default
ortrun search `.env` file on the current folder.

## License

This project is under the [MIT License](./LICENSE).

