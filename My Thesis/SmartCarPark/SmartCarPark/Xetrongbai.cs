using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SmartCarPark
{
    public partial class Xetrongbai : Form
    {
        private static string position;
        private static int remaining;
        public Xetrongbai()
        {
            InitializeComponent();
        }
        private void Xetrongbai_Load(object sender, EventArgs e)
        {
            if (position != null)
            {
                if (position[1] == '1')
                {
                    button_a1.BackColor = Color.Red;
                }
                else button_a1.BackColor = Color.Transparent;
                if (position[2] == '1')
                {
                    button_a2.BackColor = Color.Red;
                }
                else button_a2.BackColor = Color.Transparent;
                if (position[3] == '1')
                {
                    button_a3.BackColor = Color.Red;
                }
                else button_a3.BackColor = Color.Transparent;
                if (position[4] == '1')
                {
                    button_a4.BackColor = Color.Red;
                }
                else button_a4.BackColor = Color.Transparent;

                textBox_remaining.Text = remaining.ToString();
            }
        }

        public int Xecontrong(string str)
        {
            position = str;
            remaining = 0;
            foreach (var item in position)
            {
                if (item == '0')
                {
                    remaining++;
                }
            }
            return remaining;
        }
    }
}
