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
    public partial class Quanlythe : Form
    {
        public static string confirm_in;
        public static string confirm_out;
        public static string id_in_compared;
        public static string id_out_compared;
        public static string time_in;
        public static string time_out;
        public static bool id_in_temp;
        public static bool id_out_temp;

        SqlConnection connection;
        SqlCommand command;
        string str = "Data Source=LAPTOP-RAOE5KBA\\KIENPHAN;Initial Catalog=QLthe;Integrated Security=True;Encrypt=True;TrustServerCertificate=True";
        SqlDataAdapter adapter = new SqlDataAdapter();
        DataTable table = new DataTable();

        void Loaddata()
        {
            command = connection.CreateCommand();
            command.CommandText = "select * from Danhsachthe ";
            adapter.SelectCommand = command;
            table.Clear();
            adapter.Fill(table);
            dataGridView_quanlythe.DataSource = table;

        }
        public Quanlythe()
        {
            InitializeComponent();
        }

        private void Quanlythe_Load(object sender, EventArgs e)
        {
            connection = new SqlConnection(str);
            connection.Open();
            Loaddata();
        }

        private void dataGridView_quanlythe_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            int i;
            i = dataGridView_quanlythe.CurrentRow.Index;
            textBox_id.Text = dataGridView_quanlythe.Rows[i].Cells[1].Value.ToString().Trim();
        }

        private void button_them_Click(object sender, EventArgs e)
        {
            command = connection.CreateCommand();
            command.CommandText = "insert into Danhsachthe(id,ngaythem) values('" + textBox_id.Text + "', CONVERT(nvarchar(10), GETDATE(), 103))";
            command.ExecuteNonQuery();
            Loaddata();
        }

        private void button_xoa_Click(object sender, EventArgs e)
        {
            command = connection.CreateCommand();
            command.CommandText = "delete from Danhsachthe where id = '" + textBox_id.Text + "'";
            command.ExecuteNonQuery();
            Loaddata();
        }
        public void CompareID_In(string id)
        {
            //Lichsu lichsu = new Lichsu();
            connection = new SqlConnection(str);
            connection.Open();
            Loaddata();
            confirm_in = "N";
            id_in_temp = false;
            id_in_compared = "Thẻ không có trong hệ thống";
            try
            {
                foreach (DataGridViewRow row in dataGridView_quanlythe.Rows)
                {
                    try
                    {
                        if (row.Cells[1].Value != null)
                        {
                            string idValue = row.Cells[1].Value.ToString().Trim();
                            if (id == idValue)
                            {
                                id_in_compared = idValue;
                                id_in_temp = true;
                                break;
                            }
                        }

                    }
                    catch (NullReferenceException)
                    {

                        MessageBox.Show("Lỗi: ");

                    }
                    catch (Exception ex)
                    {

                        MessageBox.Show("Lỗi: " + ex.Message);
                    }
                }
            }
            catch (Exception ex)
            {

                MessageBox.Show("Lỗi: " + ex.Message);
            }

        }
        public void CompareID_Out(string id)
        {
            connection = new SqlConnection(str);
            connection.Open();
            Loaddata();
            confirm_out = "N";
            id_out_temp = false;
            id_out_compared = "Thẻ không có trong hệ thống";
            try
            {
                foreach (DataGridViewRow row in dataGridView_quanlythe.Rows)
                {
                    try
                    {
                        if (row.Cells[1].Value != null)
                        {
                            string idValue = row.Cells[1].Value.ToString().Trim();
                            if (id == idValue)
                            {
                                id_out_compared = idValue;
                                id_out_temp = true;
                                break;
                            }
                        }

                    }
                    catch (NullReferenceException)
                    {

                        MessageBox.Show("Lỗi: ");

                    }
                    catch (Exception ex)
                    {

                        MessageBox.Show("Lỗi: " + ex.Message);
                    }
                }
            }
            catch (Exception ex)
            {

                MessageBox.Show("Lỗi: " + ex.Message);
            }

        }

    }
}
