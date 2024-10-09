#compdef sp
#autoload

_sp () {
  local cmd
	if (( CURRENT > 2)); then
		cmd=${words[2]}
		curcontext="${curcontext%:*:*}:pass-$cmd"
		(( CURRENT-- ))
		shift words
		case "${cmd}" in
			-i)
				_sp_complete_keys
				;;
			-i|-y|-d|-a|-e|-o|-O)
				_sp_complete_entries_helper
				;;
			-g)
				local -a subcommands
				subcommands=(
					"secure:英数字＋特別文字（デフォルト）"
					"risk:英数字のみ（不安）"
				)
				;;
			-s)
				_sp_cmd_show
				;;
		esac
	else
		local -a subcommands
		subcommands=(
      "-a:パスワードを追加"
      "-b:漏洩されたパスワードを確認（インターネット接続が必須）"
      "-c:パスワードの長さ、強さ、や服数度を確認する。length＝パスワードの長さの確認、strength＝パスワードの強さの確認、duplicate＝服数パスワードが同じかどうか確認、all＝上記の全部の確認"
      "-d:パスワードを削除"
      "-e:パスワードを変更"
      "-g:希望文字数でパスワードをランダムに作成する。デフォルトな長さは64文字です。secure＝英数字＋特別文字（デフォルト）を使用、risk＝英数字のみ（不安）を使用"
			"-i:GPGと使ってパスワードストレージを初期設定"
      "-l:パスワード一覧を表示"
      "-o:ワンタイムパスワード（TOTP）を表示"
      "-O:ワンタイムパスワード（TOTP）をコピー。デフォルトは30秒です。"
			"-s:パスワードを表示"
      "-v:バージョンを表示"
      "-y:パスワードを表示せずクリップボードにコピーする（Xorgのみ）。デフォルトは45秒です。"
		)
		_sp_cmd_show
	fi
}

_sp_cmd_show () {
  _sp_complete_entries
}

_sp_complete_entries() {
  _sp_complete_entries_helper -type f
}

_sp_complete_entries_helper () {
  local IFS=$'\n'
  local prefix
	zstyle -s ":completion:${curcontext}:" prefix prefix || prefix="${SP_DIR:-$HOME/.local/share/sp}"
	_values -C 'passwords' ${$(find -L "$prefix" \( -name .git -o -name .gpg-id \) -prune -o $@ -print 2>/dev/null | sed -e "s#${prefix}/\{0,1\}##" -e 's#\.gpg##' -e 's#\\#\\\\#g' -e 's#:#\\:#g' | sort):-""}
}

_sp_complete_keys () {    
  local IFS=$'\n'
  _values 'gpg keys' $(gpg2 --list-secret-keys --with-colons | cut -d : -f 10 | sort -u | sed '/^$/d')
}

_sp
