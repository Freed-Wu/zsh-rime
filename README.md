# zsh-rime

![IME](https://github.com/Freed-Wu/zsh-rime/assets/32936898/4b569c55-97e3-4a73-99c3-a8daaeaa6a7d)

![complete](https://github.com/Freed-Wu/zsh-rime/assets/32936898/589d588c-05c0-4ae0-8708-9791d4221d0a)

A video can be found [here](https://asciinema.org/a/660633).

## Dependencies

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [librime](https://github.com/rime/librime)
- [make](https://www.gnu.org/software/make/)
- A C compiler

## Install

### [zinit](https://github.com/zdharma-continuum/zinit/)

```zsh
zinit id-as depth'1' wait lucid \
  if'pkg-config --libs rime &>/dev/null || (( $+commands[nix] ))' \
  atload'bindkey "^[^I" rime-complete
bindkey "^[^N" rime-next-schema
bindkey "^[^P" rime-previous-schema
bindkey "^^" rime-ime' \
  for Freed-Wu/zsh-rime
```

### Manual

```zsh
source /the/path/of/*.plugin.zsh
bindkey "^[^I" rime-complete
bindkey "^[^N" rime-next-schema
bindkey "^[^P" rime-previous-schema
bindkey "^^" rime-ime
```

## Configure

```zsh
zstyle -s ":rime:build" cppflags cppf || cppf="-I/usr/local/include"
zstyle -s ":rime:build" cflags cf || cf="-Wall -O2 -g"
zstyle -s ":rime:build" ldflags ldf || ldf="-L/usr/local/lib"
zstyle -a ':rime:ui' indices indices || indices=(① ② ③ ④ ⑤ ⑥ ⑦ ⑧ ⑨ ⓪)
zstyle -s ':rime:ui' left left || left='<|'
zstyle -s ':rime:ui' right right || right='|>'
zstyle -s ':rime:ui' left-sep left_sep || left_sep='['
zstyle -s ':rime:ui' right-sep right_sep || right_sep=']'
zstyle -s ':rime:ui' cursor cursor || cursor='|'
zstyle -s ':rime:ui' prompt-len prompt_len || prompt_len=2
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
