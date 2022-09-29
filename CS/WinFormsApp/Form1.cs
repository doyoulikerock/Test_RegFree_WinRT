using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WinFormsApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Button1_Click(object sender, EventArgs e)
        {

            WinRTComponent.Class winRTClass = new WinRTComponent.Class();
            String textFromComponent = winRTClass.MyProperty.ToString();
            TextBox1.Text = textFromComponent;

            var popup = new Popup(textFromComponent);
            popup.Show(this);
        }

        private void TextBox1_TextChanged(object sender, EventArgs e)
        {

        }


        WASAPI.Class wasClass = null;
        private void button2_Click(object sender, EventArgs e)
        {
            if(wasClass == null)
                wasClass = new WASAPI.Class();
            string text = wasClass.MyProperty.ToString();

            wasClass.Init(0);


        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (wasClass == null)
                wasClass = new WASAPI.Class();
            string text = wasClass.MyProperty.ToString();

            wasClass.Init(1);


            WASAPI.Env env = new WASAPI.Env();
            env.Prm1 = 2;
            //int env_prm2 = WASAPI.Env.Method1();
            int ret = env.Prm1;
        }
    }
}
