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
    public partial class Doanhthu : Form
    {
        public static string phiguixe= "5000";
        public Doanhthu()
        {
            InitializeComponent();
        }

        private void Doanhthu_Load(object sender, EventArgs e)
        {
            dateTimePicker_from.MaxDate = DateTime.Now;
            dateTimePicker_to.MaxDate = DateTime.Now;
            dateTimePicker_from.Value = DateTime.Today;
            dateTimePicker_to.Value = DateTime.Today;
        }

        private void comboBox_phithu_SelectedIndexChanged(object sender, EventArgs e)
        {
            button_phiguixe.Enabled = true;
        }

        private void button_phithu_Click(object sender, EventArgs e)
        {
            phiguixe = comboBox_phithu.Text;
            button_phiguixe.Enabled = false;
        }

        private void button_tinhdoanhthu_Click(object sender, EventArgs e)
        {
            Lichsu ls = new Lichsu();
            DateTime from = dateTimePicker_from.Value;
            DateTime to = dateTimePicker_to.Value;
            to = to.AddDays(1);
            int doanhthu = ls.Tinhdoanhthu(from,to);
            textBox_doanhthu.Text = doanhthu.ToString();
        }

    }
}
