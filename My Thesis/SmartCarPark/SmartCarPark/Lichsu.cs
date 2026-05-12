using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace SmartCarPark
{
    public partial class Lichsu : Form
    {
        public static TimeSpan time_guixe;
        public static bool match;
        //public static int total;

        SqlConnection connection_in;
        SqlConnection connection_out;
        SqlCommand command_in;
        SqlCommand command_out;
        string str = "Data Source=LAPTOP-RAOE5KBA\\KIENPHAN;Initial Catalog=QLthe;Integrated Security=True;Encrypt=True;TrustServerCertificate=True";
        SqlDataAdapter adapter_in = new SqlDataAdapter();
        SqlDataAdapter adapter_out = new SqlDataAdapter();
        DataTable table_in = new DataTable();
        DataTable table_out = new DataTable();

        void Loaddata_in()
        {
            command_in = connection_in.CreateCommand();
            command_in.CommandText = "select * from Lichsuvao";
            adapter_in.SelectCommand = command_in;
            table_in.Clear();
            adapter_in.Fill(table_in);
            dataGridView_in.DataSource = table_in;
        }
        void Loaddata_out()
        {
            command_out = connection_out.CreateCommand();
            command_out.CommandText = "select * from Lichsura";
            adapter_out.SelectCommand = command_out;
            table_out.Clear();
            adapter_out.Fill(table_out);
            dataGridView_out.DataSource = table_out;
        }
        public Lichsu()
        {
            InitializeComponent();
        }

        private void Lichsu_Load(object sender, EventArgs e)
        {
            connection_in = new SqlConnection(str);
            connection_out = new SqlConnection(str);
            connection_in.Open();
            connection_out.Open();
            Loaddata_in();
            Loaddata_out();
        }

        public void Lichsu_vao()
        {
            connection_in = new SqlConnection(str);
            connection_in.Open();
            Loaddata_in();
            string id_vao = Giaodien.id_ls_vao;
            string bienso_vao = Giaodien.bienso_ls_vao;
            string time_vao = Giaodien.time_ls_vao;
            command_in = connection_in.CreateCommand();
            command_in.CommandText = "insert into Lichsuvao(id,bienso,datetime) values('" + id_vao + "', '" + bienso_vao + "','" + time_vao + "')";
            command_in.ExecuteNonQuery();

        }
        public void Lichsu_ra()
        {
            connection_out = new SqlConnection(str);
            connection_out.Open();
            Loaddata_out();
            connection_in = new SqlConnection(str);
            connection_in.Open();
            Loaddata_in();
            match = false;
            string id_ra = Giaodien.id_ls_ra;
            string bienso_ra = Giaodien.bienso_ls_ra;
            string time_ra = Giaodien.time_ls_ra;
            int i = dataGridView_in.RowCount - 1;
            for (; i >= 0; i--) 
            {
                if (id_ra == dataGridView_in.Rows[i].Cells[1].Value?.ToString().Trim() && 
                    bienso_ra == dataGridView_in.Rows[i].Cells[2].Value?.ToString().Trim())
                {
                    match=true;
                    DateTime timera_datetime = DateTime.Parse(time_ra);
                    DateTime timeguixevao_datetime = DateTime.Parse(dataGridView_in.Rows[i].Cells[3].Value.ToString());
                    time_guixe = timera_datetime - timeguixevao_datetime;
                    string phi = Doanhthu.phiguixe;
                    command_out = connection_out.CreateCommand();
                    command_out.CommandText = "insert into Lichsura(id,bienso,datetime,phiguixe) values('" + id_ra + "', '" + bienso_ra + "','" + time_ra + "','" + phi + "')";
                    command_out.ExecuteNonQuery();
                    break;
                }
                if (i == 0)
                {
                    match = false;
                }
            }
            
        }
        public int Tinhdoanhthu(DateTime fromdate, DateTime todate)
        {
            connection_out = new SqlConnection(str);
            connection_out.Open();
            Loaddata_out();
            int total = 0;
            foreach(DataGridViewRow row in dataGridView_out.Rows)
            {
                DateTime datadate = Convert.ToDateTime(row.Cells[3].Value);
                if(datadate >= fromdate && datadate <= todate)
                {
                    total += Convert.ToInt32(row.Cells[4].Value);
                }
            }
            return total;
        }

        private void dataGridView_in_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            int i;
            i = dataGridView_in.CurrentRow.Index;
            textBox_id_vao.Text = dataGridView_in.Rows[i].Cells[1].Value.ToString().Trim();
            textBox_bienso_vao.Text = dataGridView_in.Rows[i].Cells[2].Value.ToString();
            textBox_time_vao.Text = dataGridView_in.Rows[i].Cells[3].Value.ToString();
        }

        private void dataGridView_out_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            int i;
            i = dataGridView_out.CurrentRow.Index;
            textBox_id_ra.Text = dataGridView_in.Rows[i].Cells[1].Value.ToString().Trim();
            textBox_bienso_ra.Text = dataGridView_in.Rows[i].Cells[2].Value.ToString();
            textBox_time_ra.Text = dataGridView_in.Rows[i].Cells[3].Value.ToString();

        }
    }
}
