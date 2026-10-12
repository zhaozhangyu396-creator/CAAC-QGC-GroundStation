# Read Unicode shortcut filenames through the native Shell interface.
if (-not ('QgcWindowsShortcut' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;

[ComImport, Guid("00021401-0000-0000-C000-000000000046")]
internal class QgcShellLink { }

[ComImport, Guid("000214F9-0000-0000-C000-000000000046"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
internal interface QgcShellLinkW {
    void GetPath([Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder value, int capacity, IntPtr data, uint flags);
    void GetIDList(out IntPtr value);
    void SetIDList(IntPtr value);
    void GetDescription([Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder value, int capacity);
    void SetDescription([MarshalAs(UnmanagedType.LPWStr)] string value);
    void GetWorkingDirectory([Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder value, int capacity);
    void SetWorkingDirectory([MarshalAs(UnmanagedType.LPWStr)] string value);
    void GetArguments([Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder value, int capacity);
    void SetArguments([MarshalAs(UnmanagedType.LPWStr)] string value);
    void GetHotkey(out short value);
    void SetHotkey(short value);
    void GetShowCmd(out int value);
    void SetShowCmd(int value);
    void GetIconLocation([Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder value, int capacity, out int index);
    void SetIconLocation([MarshalAs(UnmanagedType.LPWStr)] string value, int index);
    void SetRelativePath([MarshalAs(UnmanagedType.LPWStr)] string value, uint reserved);
    void Resolve(IntPtr window, uint flags);
    void SetPath([MarshalAs(UnmanagedType.LPWStr)] string value);
}

public sealed class QgcShortcutData {
    public string TargetPath { get; internal set; }
    public string WorkingDirectory { get; internal set; }
    public string Arguments { get; internal set; }
    public string IconLocation { get; internal set; }
}

public static class QgcWindowsShortcut {
    public static QgcShortcutData Read(string filename) {
        var instance = new QgcShellLink();
        try {
            ((IPersistFile)instance).Load(filename, 0);
            var link = (QgcShellLinkW)instance;
            var value = new StringBuilder(32768);
            var result = new QgcShortcutData();
            link.GetPath(value, value.Capacity, IntPtr.Zero, 4);
            result.TargetPath = value.ToString();
            value.Clear();
            link.GetWorkingDirectory(value, value.Capacity);
            result.WorkingDirectory = value.ToString();
            value.Clear();
            link.GetArguments(value, value.Capacity);
            result.Arguments = value.ToString();
            value.Clear();
            int index;
            link.GetIconLocation(value, value.Capacity, out index);
            result.IconLocation = value.ToString() + "," + index;
            return result;
        } finally {
            Marshal.FinalReleaseComObject(instance);
        }
    }
}
'@
}
