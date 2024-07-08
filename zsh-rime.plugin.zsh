# https://github.com/zdharma-continuum/Zsh-100-Commits-Club/blob/master/Zsh-Plugin-Standard.adoc
0="${${ZERO:-${0:#$ZSH_ARGZERO}}:-${(%):-%N}}"
0="${${(M)0:#/*}:-$PWD/$0}"

RIME_REPO_DIR="${0:h}"

if [[ "${fpath[(I)$RIME_REPO_DIR]}" == 0 ]]; then
    fpath+=( "$RIME_REPO_DIR" )
fi

autoload -Uz rime-compile

if [ ! -e "${RIME_REPO_DIR}/module/Src/zi/rime.so" ]; then
    builtin print "${fg_bold[magenta]}zi${reset_color}/${fg_bold[yellow]}rime${reset_color} is building..."
    rime-compile $RIME_REPO_DIR
elif [[ ! -f "${RIME_REPO_DIR}/module/COMPILED_AT" || ( "${RIME_REPO_DIR}/module/COMPILED_AT" -ot "${RIME_REPO_DIR}/module/RECOMPILE_REQUEST" ) ]]; then
    # Don't trust access times and verify hard stored values
    [[ -e ${RIME_REPO_DIR}/module/COMPILED_AT ]] && local compiled_at_ts="$(<${RIME_REPO_DIR}/module/COMPILED_AT)"
    [[ -e ${RIME_REPO_DIR}/module/RECOMPILE_REQUEST ]] && local recompile_request_ts="$(<${RIME_REPO_DIR}/module/RECOMPILE_REQUEST)"

    if [[ "${recompile_request_ts:-1}" -gt "${compiled_at_ts:-0}" ]]; then
        builtin echo "${fg_bold[red]}rime: single recompiletion requested by plugin's update${reset_color}"
        rime-compile $RIME_REPO_DIR
    fi
fi

# Finally load the module - if it has compiled
if [[ -e "${RIME_REPO_DIR}/module/Src/zi/rime.so" ]]; then
    MODULE_PATH="${RIME_REPO_DIR}/module/Src":"$MODULE_PATH"
    zmodload zi/rime

    rime init &&
      rime createSession rime_session_id &&
      rime getCurrentSchema $rime_session_id rime_schema_id &&
      rime getSchemaList $rime_schema_list
    autoload -Uz rime-complete &&
      zle -C rime-complete expand-or-complete rime-complete
    autoload -Uz rime-next-schema &&
      zle -N rime-next-schema
    autoload -Uz rime-previous-schema &&
      zle -N rime-previous-schema

    autoload -Uz rime-ime &&
      zle -N rime-ime
fi

unset RIME_REPO_DIR
