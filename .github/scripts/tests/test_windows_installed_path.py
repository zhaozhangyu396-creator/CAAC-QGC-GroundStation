"""Installed boot checks must not resolve missing package DLLs from build SDKs."""

from __future__ import annotations

import pytest
from windows_installed_path import filter_sdk_paths, main


def test_mixed_slashes_and_case_remove_both_build_sdks():
    path = (
        r"C:\Windows\System32;D:/cache/GStreamer/sdk/bin;d:\qt\6.11.1\bin;"
        r"D:/cache/GStreamer/sdk/lib/plugins;D:\other\bin"
    )
    assert filter_sdk_paths(path, [r"D:\cache\gstreamer\sdk", "D:/Qt/6.11.1"]) == (
        r"C:\Windows\System32;D:\other\bin"
    )


def test_quoted_paths_trailing_slashes_and_dot_segments():
    path = r'"D:\Program Files\Qt\bin\";D:/SDK/./bin/;D:\SDK\bin\..\lib;C:\Windows'
    assert filter_sdk_paths(path, ["D:/Program Files/Qt/", r"D:\SDK"]) == r"C:\Windows"


def test_neighboring_prefixes_and_other_drives_are_retained():
    path = r"D:\Qt\bin;D:\QtBackup\bin;D:\Qt-other\bin;C:\Qt\bin"
    assert filter_sdk_paths(path, [r"D:\Qt"]) == r"D:\QtBackup\bin;D:\Qt-other\bin;C:\Qt\bin"


def test_empty_optional_sdk_does_not_remove_system_paths():
    assert filter_sdk_paths(r";C:\Windows;;D:\tools;", [""]) == r"C:\Windows;D:\tools"


def test_path_line_break_is_rejected():
    with pytest.raises(ValueError, match="line break"):
        filter_sdk_paths("C:\\Windows\nUNEXPECTED=value", [r"D:\Qt"])


def test_entrypoint_preserves_existing_environment_file(tmp_path, monkeypatch, capsys):
    output = tmp_path / "github-env"
    output.write_text("EXISTING=value\n", encoding="utf-8")
    monkeypatch.setenv("GITHUB_ENV", str(output))
    monkeypatch.setenv("QT_ROOT", "D:/Qt")
    monkeypatch.setenv("GST_ROOT", r"D:\GST")
    monkeypatch.setenv("PATH", r"D:\Qt\bin;D:/GST/bin;C:\Windows;D:\tools")
    assert main() == 0
    assert output.read_text(encoding="utf-8") == (
        "EXISTING=value\nQGC_VERIFY_INSTALLED_PATH=C:\\Windows;D:\\tools\n"
    )
    assert "C:\\Windows" not in capsys.readouterr().out


def test_missing_qt_root_fails_without_saving_a_path(tmp_path, monkeypatch):
    output = tmp_path / "github-env"
    monkeypatch.setenv("GITHUB_ENV", str(output))
    monkeypatch.delenv("QT_ROOT", raising=False)
    assert main() == 1
    assert not output.exists()
