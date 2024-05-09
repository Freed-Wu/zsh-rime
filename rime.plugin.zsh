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

#
# Compile the module
#

rime_compile() {
    # Get CPPFLAGS, CFLAGS, LDFLAGS
    local cppf cf ldf
    zstyle -s ":plugin:rime" cppflags cppf || cppf="-I/usr/local/include"
    zstyle -s ":plugin:rime" cflags cf || cf="-Wall -O2 -g"
    zstyle -s ":plugin:rime" ldflags ldf || ldf="-L/usr/local/lib"

    (
        local build=1
        zmodload zsh/system && { zsystem flock -t 1 "${RIME_REPO_DIR}/module/configure.ac" || build=0; }
        if (( build )); then
            builtin cd "${RIME_REPO_DIR}/module"
            CPPFLAGS="$cppf" CFLAGS="$cf" LDFLAGS="$ldf" ./configure
            command make clean
            command make

            local ts="$EPOCHSECONDS"
            [[ -z "$ts" ]] && ts=$( date +%s )
            builtin echo "$ts" >! "${RIME_REPO_DIR}/module/COMPILED_AT"
        fi
    )
}

if [ ! -e "${RIME_REPO_DIR}/module/Src/zi/rime.so" ]; then
    builtin print "${fg_bold[magenta]}zi${reset_color}/${fg_bold[yellow]}rime${reset_color} is building..."
    rime_compile
elif [[ ! -f "${RIME_REPO_DIR}/module/COMPILED_AT" || ( "${RIME_REPO_DIR}/module/COMPILED_AT" -ot "${RIME_REPO_DIR}/module/RECOMPILE_REQUEST" ) ]]; then
    # Don't trust access times and verify hard stored values
    [[ -e ${RIME_REPO_DIR}/module/COMPILED_AT ]] && local compiled_at_ts="$(<${RIME_REPO_DIR}/module/COMPILED_AT)"
    [[ -e ${RIME_REPO_DIR}/module/RECOMPILE_REQUEST ]] && local recompile_request_ts="$(<${RIME_REPO_DIR}/module/RECOMPILE_REQUEST)"

    if [[ "${recompile_request_ts:-1}" -gt "${compiled_at_ts:-0}" ]]; then
        builtin echo "${fg_bold[red]}rime: single recompiletion requested by plugin's update${reset_color}"
        rime_compile
    fi
fi

# Finally load the module - if it has compiled
if [[ -e "${RIME_REPO_DIR}/module/Src/zi/rime.so" ]]; then
    MODULE_PATH="${RIME_REPO_DIR}/module/Src":"$MODULE_PATH"
    zmodload zi/rime
    rime -C
    _rime_context() {
        rime -p nihk
	local -a candidates results=($(rime -c))
	while (( $#results )); do
	    candidates+=($results)
	    rime -p =
	    results=($(rime -c))
	done
        _values "$(rime -g)" $candidates
        rime -p
    }
    _rime_get_context() {
      eval "$_comp_setup"
      _main_complete _rime_context
    }
    zle -C rime-get-context expand-or-complete _rime_get_context
fi

unset RIME_REPO_DIR
