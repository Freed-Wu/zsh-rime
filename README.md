# zsh-rime

![screenshot](https://github.com/Freed-Wu/zsh-rime/assets/32936898/2dbaede6-4047-4fa7-9c39-9a7cca98b3bf)

## Dependencies

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [make](https://www.gnu.org/software/make/)
- [librime](https://github.com/rime/librime)

## Install

### [zinit](https://github.com/zdharma-continuum/zinit/)

```zsh
zinit id-as depth'1' wait lucid \
  if'(($+commands[rime_deployer]))' \
  atload'bindkey "^[^I" rime-get-context' \
  for Freed-Wu/zsh-rime
```

### Manual

```zsh
source /the/path/of/*.plugin.zsh
```

## Config

```zsh
zstyle -s ":plugin:rime" cppflags cppf || cppf="-I/usr/local/include"
zstyle -s ":plugin:rime" cflags cf || cf="-Wall -O2 -g"
zstyle -s ":plugin:rime" ldflags ldf || ldf="-L/usr/local/lib"
```

## Usage

```zsh
bindkey '^[^I' rime-get-context
```
