# zsh-rime

![IME](https://github.com/Freed-Wu/zsh-rime/assets/32936898/4b569c55-97e3-4a73-99c3-a8daaeaa6a7d)

![complete](https://github.com/Freed-Wu/zsh-rime/assets/32936898/589d588c-05c0-4ae0-8708-9791d4221d0a)

A video can be found [here](https://asciinema.org/a/660633).

## Dependencies

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [librime](https://github.com/rime/librime)
- [make](https://www.gnu.org/software/make/)
- A C compiler

```sh
# Ubuntu
sudo apt-get -y install librime-dev librime1 pkg-config make
sudo apt-mark auto librime-dev pkg-config make
# ArchLinux
sudo pacman -S --noconfirm librime pkg-config make
# Android Termux
apt-get -y install librime pkg-config make
# Nix
# without any extra operation
# homebrew
brew tap tonyfettes/homebrew-rime
brew install librime pkg-config make
# Windows msys2
pacboy -S --noconfirm pkg-config librime gcc make
```

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

zstyle -a ':rime:traits' shared-data-dir shared_data_dir ||
  for dir in ${PREFIX:-/usr}/share/rime-data /usr/local/share/rime-data /run/current-system/sw/share/rime-data /sdcard/rime-data; do
    [[ -d $dir ]] &&
      shared_data_dir=$dir
  done
zstyle -a ':rime:traits' user-data-dir user_data_dir ||
  for dir in $HOME/.config/ibus/rime $HOME/.local/share/fcitx5/rime $HOME/.config/fcitx/rime /sdcard/rime; do
    [[ -d $dir ]] &&
      user_data_dir=$dir
  done
zstyle -s ':rime:traits' log-dir log_dir ||
  log_dir=${TMPPREFIX:-/tmp/zsh}/rime
zstyle -s ':rime:traits' distribution-name distribution_name ||
  distribution_name=Rime
zstyle -s ':rime:traits' distribution-code-name distribution_code_name ||
  distribution_code_name=zsh-rime
zstyle -s ':rime:traits' distribution-version distribution_version ||
  distribution_version=0.0.1
zstyle -s ':rime:traits' app-name app_name ||
  app_name=rime.zsh-rime
zstyle -s ':rime:traits' min-log-level min_log_level ||
  min_log_level=3

zstyle -a ':rime:keys' special special ||
  special=('^'{\?.._} '^['{\ ..~} '^[['{A..D}) &&
  zmodload zsh/terminfo &&
  for key in k{u,d,l,r,D,I,P,N,h,\;} @7 k{1..9} F{1..9} F{A..P}; do
    special+=("$termcap[$key]")
  done &&
  zmodload zsh/termcap &&
  for key in kcu{u,d,b,f}1 k{d,i}ch1 k{p,n}p k{home,end} kf{1..35}; do
    special+=("$terminfo[$key]")
  done
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
