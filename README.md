# ortrun

A lightweight command runner with env file support and command shortcuts, written in C.

## Features

- Run commands with custom environment files
- Save frequently used commands with shortcuts
- List all saved commands
- Verbose output for debugging
- Default `.env` file support

## Setup

Clone the repository:

```bash
git clone git@github.com:saracalihan/ortrun.git
cd ortrun
```

Build the project:

```bash
make
```

Now `ortrun` is ready to use.

## Usage

### Basic Usage

Run a command with the default `.env` file:

```bash
ortrun node server.js
```

Run a command with a custom env file:

```bash
ortrun -f test/.env bash test/test.sh
```

### Command Shortcuts

Add a command shortcut:

```bash
ortrun -a myapi -f .env.prod node server.js
```

Run a saved command:

```bash
ortrun -r myapi
```

List all saved commands:

```bash
ortrun -l
```

### Options

- `-f <path>`: Specify env file path (default: `.env`)
- `-a <name>`: Add command with a shortcut name
- `-r <name>`: Run a saved command by name
- `-l`: List all saved commands
- `-v`: Verbose mode - shows env path and final command
- `-h`: Show help

### Example Env File

```bash
# Auth service env
DATABASE_URL=postgres://localhost:5432/mydb
API_KEY=your_secret_key #inline comment
PORT=3000
```

### Examples

```bash
# Run with verbose output
ortrun -v -f .env.prod npm start

# Add a development server shortcut
ortrun -a dev -f .env.dev npm run dev

# Add a production deployment shortcut
ortrun -a deploy -f .env.prod ./deploy.sh

# List all saved shortcuts
ortrun -l

# Run saved shortcuts
ortrun -r dev
ortrun -r deploy
```

## License

This project is licensed under the [MIT License](./LICENSE).

## Authors

- Alihan SARAC
