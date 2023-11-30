# compdef sp

_sp () {
  local state
  local -a options
  local -a entries

  _arguments -C \
    '-i[GPGと使ってパスワードストレージを初期設定]: :_sp_complete_keys' \
    '-s[パスワードを表示]: :_sp_complete_entries' \
    '-y[パスワードを表示せずクリップボードにコピーする]: :_sp_complete_entries' \
    '-l[パスワード一覧を表示]' \
    '-a[パスワードを追加]: :_sp_complete_entries' \
    '-d[パスワードを削除]: :_sp_complete_entries' \
    '-g[希望文字数でパスワードをランダムに作成する]: :_sp_complete_entries' \
    '-o[ワンタイムパスワード（TOTP）を表示]: :_sp_complete_entries' \
    '-h[ヘルプを表示]' \
    '-v[バージョンを表示]' \
    '*:: :->subcmds'
    
  case $state in
    subcmds)
      _message "no more arguments"
      ;;
  esac
}

_sp_cmd_show () {
  _sp_complete_entries
}

_sp_complete_entries() {
  _sp_complete_entries_helper -type f
}

_sp_complete_entries_helper () {
  local IFS=$'\n'
  local prefix="${SP_DIR:-$HOME/.local/share/sp}"
  entries=("${(f)$(find -L "$prefix" \( -name .git -o -name .gpg-id \) -prune -o -type f -print 2>/dev/null | sed -e "s#${prefix}/##" -e 's#\.gpg$##')}")
  _describe 'password entries' entries
}

_sp_complete_keys () {    
  local IFS=$'\n'
  _values 'gpg keys' $(gpg2 --list-secret-keys --with-colons | cut -d : -f 10 | sort -u | sed '/^$/d')
}

compdef _sp sp
