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

        public Form1(string[] args)
        {
            from = args[0].Replace('/','\\');
            to = args[1].Replace('/', '\\');
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (!to.EndsWith("\\"))
                to += "\\";
            Process.Start(to + "Lichtsteuerung.exe");
            this.Dispose();
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
                try
                {
                    Directory.Delete(to, true);
                }
                catch (Exception ex)
                {
                    label_delete.Invoke((MethodInvoker)delegate {
                        label_delete.Text = "\u274C";
                    });
                    MessageBox.Show(ex.Message, "Fehler beim löschen.");
                    return;
                }
                label_delete.Invoke((MethodInvoker)delegate {
                    label_delete.Text = "\u2713";
                });
                label_info_copy.Invoke((MethodInvoker)delegate {
                    label_info_copy.ForeColor = Color.Black;
                });
                string rootDir = to.Substring(0, to.LastIndexOf('\\'));
                string targetName = to.Substring(to.LastIndexOf('\\') + 1);
                string fromName = from.Substring(from.LastIndexOf('\\') + 1);
                try
                {
                    Directory.Move(from, to);
                }
                catch (Exception ex)
                {
                    label_copy.Invoke((MethodInvoker)delegate {
                        label_copy.Text = "\u274C";
                    });
                    MessageBox.Show(ex.Message, "Fehler beim kopieren.");
                    return;
                }
                label_copy.Invoke((MethodInvoker)delegate {
                    label_copy.Text = "\u2713";
                });
                button_start.Invoke((MethodInvoker)delegate {
                    button_start.Enabled = true;
                });
            });
            thread.IsBackground = true;
            thread.Start();
        }
    }
}
