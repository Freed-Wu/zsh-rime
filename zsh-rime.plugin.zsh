#
# No plugin manager is needed to use this file. All that is needed is adding:
#   source {where-rime-is}/rime.plugin.zsh
#
# to ~/.zshrc.
#

0="${(%):-%N}" # this gives immunity to functionargzero being unset
RIME_REPO_DIR="${0%/*}"

#
# Update FPATH if:
# 1. Not loading with ZI
# 2. Not having fpath already updated (that would equal: using other plugin manager)
#

if [[ -z "$ZPLG_CUR_PLUGIN" && "${fpath[(r)$RIME_REPO_DIR]}" != $RIME_REPO_DIR ]]; then
    fpath+=( "$RIME_REPO_DIR" )
fi

[[ -z "${fg_bold[green]}" ]] && builtin autoload -Uz colors && colors

autoload -Uz rime-compile

if [ ! -e "${RIME_REPO_DIR}/module/Src/zi/rime.so" ]; then
    builtin print "${fg_bold[magenta]}zi${reset_color}/${fg_bold[yellow]}rime${reset_color} is building..."
    rime-compile
elif [[ ! -f "${RIME_REPO_DIR}/module/COMPILED_AT" || ( "${RIME_REPO_DIR}/module/COMPILED_AT" -ot "${RIME_REPO_DIR}/module/RECOMPILE_REQUEST" ) ]]; then
    # Don't trust access times and verify hard stored values
    [[ -e ${RIME_REPO_DIR}/module/COMPILED_AT ]] && local compiled_at_ts="$(<${RIME_REPO_DIR}/module/COMPILED_AT)"
    [[ -e ${RIME_REPO_DIR}/module/RECOMPILE_REQUEST ]] && local recompile_request_ts="$(<${RIME_REPO_DIR}/module/RECOMPILE_REQUEST)"

    if [[ "${recompile_request_ts:-1}" -gt "${compiled_at_ts:-0}" ]]; then
        builtin echo "${fg_bold[red]}rime: single recompiletion requested by plugin's update${reset_color}"
        rime-compile
    fi
fi

# Finally load the module - if it has compiled
if [[ -e "${RIME_REPO_DIR}/module/Src/zi/rime.so" ]]; then
    MODULE_PATH="${RIME_REPO_DIR}/module/Src":"$MODULE_PATH"
    zmodload zi/rime

    rime -Cgl
    autoload -Uz rime-complete
    zle -C rime-complete expand-or-complete rime-complete
    autoload -Uz rime-next-schema
    zle -N rime-next-schema
    autoload -Uz rime-previous-schema
    zle -N rime-previous-schema
fi

unset RIME_REPO_DIR
