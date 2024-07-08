# zsh-rime

![screenshot](https://github.com/Freed-Wu/zsh-rime/assets/32936898/589d588c-05c0-4ae0-8708-9791d4221d0a)

A video can be found [here](https://asciinema.org/a/660633).

## Dependencies

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [make](https://www.gnu.org/software/make/)
- [librime](https://github.com/rime/librime)

## Install

### [zinit](https://github.com/zdharma-continuum/zinit/)

```zsh
zinit id-as depth'1' wait lucid \
  if'pkg-config --libs rime &>/dev/null || (( $+commands[nix] ))' \
  atload'bindkey "^[^I" rime-complete
bindkey "^[^N" rime-next-schema
bindkey "^[^P" rime-previous-schema' \
  for Freed-Wu/zsh-rime
```

### Manual

```zsh
source /the/path/of/*.plugin.zsh
bindkey "^[^I" rime-complete
bindkey "^[^N" rime-next-schema
bindkey "^[^P" rime-previous-schema
```

## Configure

```zsh
zstyle -s ":plugin:rime" cppflags cppf || cppf="-I/usr/local/include"
zstyle -s ":plugin:rime" cflags cf || cf="-Wall -O2 -g"
zstyle -s ":plugin:rime" ldflags ldf || ldf="-L/usr/local/lib"
```

## Integrate

### [powerlevel10k](https://github.com/romkatv/powerlevel10k)

`p10k.zsh`:

```zsh
() {
  # ...
  typeset -g POWERLEVEL9K_LEFT_PROMPT_ELEMENTS=(
    # ...
    my_rime
    # ...
  )
  # ...
  function prompt_my_rime() {
    if [[ -n $rime_schema_id ]] && (( $#rime_schema_list )); then
      p10k segment -bblack -fgreen -iㄓ -t$rime_schema_list[$rime_schema_id]
    fi
  }
  # ...
}
```
