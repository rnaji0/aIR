# aIR
[![Scorecard](https://api.scorecard.dev/projects/github.com/rnaji0/aIR/badge)](https://scorecard.dev/viewer/?uri=github.com/rnaji0/aIR)

An agent's actions on a machine form a program: a stream of shell commands, file reads, process spawns, queries, and requests, produced by a model we're frankly blind to. Most agent optimization targets the model. I've recently read systems work where optimizations have focused on the machine side (faster sandboxes, speculative tool calls, compressed idle memory), but it treats each action as an isolated event.

Most agent optimization targets the model while aIR aims to target the program on the machine itself. It treats an agent's action stream the way a managed runtime treats bytecode. It's something to profile, optimize, and compile, without touching the model.

This project will be tested on firecracker and will use both general agent frameworks and RL-focused frameworks.

> Early and in active development.

This is inspired by Azul's Falcon JIT compiler and ReadyNow. The goal of this project is to do for an agent's interaction with the machine what they did for Java execution.

## License

[Apache 2.0](LICENSE)
