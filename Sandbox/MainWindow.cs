using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Sandbox
{
    public partial class MainWindow : Form
    {
        public MainWindow()
        {
            InitializeComponent();

            IntPtr pClassName = Driver_DX9.GetInstance();

            bool result = Driver_DX9.InitDriver(pClassName, this.Handle);

            System.Windows.Forms.Timer t = new System.Windows.Forms.Timer();
            t.Interval = 1000;

            t.Tick += (sender, e) => 
            {
                Driver_DX9.Begin(pClassName, 1);
                
                Driver_DX9.End(pClassName);
            };

            t.Enabled = true;
        }
    }
}
