using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace WindowsInstaller
{
    static class Program
    {
        /// <summary>
        /// Der Haupteinstiegspunkt für die Anwendung.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            // we have an open handle to the current dir. If the current dir in the release folder we can not delete this
            Environment.CurrentDirectory = "C:\\";
            if (args.Length != 2)
            {
                MessageBox.Show("Programm mit den falschen Parametern gestartet, diese sind 'from' 'to'", "Fehler");
                return;
            }
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1(args));
        }
    }
}
