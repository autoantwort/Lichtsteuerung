using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Threading;

namespace WindowsInstaller
{
    public partial class Form1 : Form
    {
        string from;
        string to;
        System.Security.AccessControl.DirectorySecurity folderAccessControl;

        enum Progress{ Wait, Remove, Copy};
        Progress progress = Progress.Wait;

        public Form1(string[] args)
        {
            from = args[0].Replace('/','\\');
            to = args[1].Replace('/', '\\');
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(progress == Progress.Copy)
            {
                if (!to.EndsWith("\\"))
                    to += "\\";
                Process.Start(to + "Lichtsteuerung.exe");
                this.Dispose();
            }
            else
            {
                button_start.Enabled = false;
                Form1_Shown(null, null);
            }
        }

        private void button_close_Click(object sender, EventArgs e)
        {
            this.Dispose();
        }

        public static bool isProcessRunning(string s)
        {
            s = s.ToLower();
            if (s == null || s.Length == 0)
            {
                return false;
            }
            foreach (Process p in Process.GetProcesses())
            {
                if (p.ProcessName.ToLower().Contains(s))
                    return true;
            }
            return false;
        }

        // https://stackoverflow.com/questions/329355/cannot-delete-directory-with-directory-deletepath-true
        public static void DeleteDirectory(string path)
        {
            foreach (string directory in Directory.GetDirectories(path))
            {
                DeleteDirectory(directory);
            }

            try
            {
                Directory.Delete(path, true);
            }
            catch (IOException)
            {
                Directory.Delete(path, true);
            }
            catch (UnauthorizedAccessException)
            {
                Directory.Delete(path, true);
            }
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            var thread = new Thread(() =>
            {
                while (isProcessRunning("lichtsteuerung"))
                {
                    Thread.Sleep(100);
                }
                label_wait.Invoke((MethodInvoker)delegate {
                    label_wait.Text = "\u2713";
                });
                label_info_delete.Invoke((MethodInvoker)delegate {
                    label_info_delete.ForeColor = Color.Black;
                });
                if(progress == Progress.Wait)
                {
                    // save the permissions of the old folder
                    folderAccessControl = Directory.GetAccessControl(to);
                    folderAccessControl.SetAccessRuleProtection(false, true);
                    try
                    {
                        DeleteDirectory(to);
                    }
                    catch (Exception ex)
                    {
                        label_delete.Invoke((MethodInvoker)delegate {
                            label_delete.Text = "\u274C";
                        });
                        MessageBox.Show(ex.Message, "Fehler beim löschen.");
                        button_start.Invoke((MethodInvoker)delegate {
                            button_start.Enabled = true;
                            button_start.Text = "Nochmal probieren";
                        });
                        return;
                    }
                    progress = Progress.Remove;
                    label_delete.Invoke((MethodInvoker)delegate {
                        label_delete.Text = "\u2713";
                    });
                    label_info_copy.Invoke((MethodInvoker)delegate {
                        label_info_copy.ForeColor = Color.Black;
                    });
                }
                if(progress == Progress.Remove)
                {
                    string rootDir = to.Substring(0, to.LastIndexOf('\\'));
                    string targetName = to.Substring(to.LastIndexOf('\\') + 1);
                    string fromName = from.Substring(from.LastIndexOf('\\') + 1);
                    try
                    {
                        Directory.Move(from, to);
                        // set the old permissions to the new folder
                        Directory.SetAccessControl(to, folderAccessControl);
                    }
                    catch (Exception ex)
                    {
                        label_copy.Invoke((MethodInvoker)delegate {
                            label_copy.Text = "\u274C";
                        });
                        MessageBox.Show(ex.Message, "Fehler beim kopieren.");
                        button_start.Invoke((MethodInvoker)delegate {
                            button_start.Enabled = true;
                            button_start.Text = "Nochmal probieren";
                        });
                        return;
                    }
                    label_copy.Invoke((MethodInvoker)delegate {
                        label_copy.Text = "\u2713";
                    });
                    button_start.Invoke((MethodInvoker)delegate {
                        button_start.Enabled = true;
                        button_start.Text = "Lichtsteuerung starten";
                    });
                    progress = Progress.Copy;
                }
                
            });
            thread.IsBackground = true;
            thread.Start();
        }
    }
}
