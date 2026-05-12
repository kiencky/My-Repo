namespace SmartCarPark
{
    partial class Quanlythe
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.dataGridView_quanlythe = new System.Windows.Forms.DataGridView();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.button_xoa = new System.Windows.Forms.Button();
            this.button_them = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_id = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_quanlythe)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // dataGridView_quanlythe
            // 
            this.dataGridView_quanlythe.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView_quanlythe.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_quanlythe.Location = new System.Drawing.Point(50, 37);
            this.dataGridView_quanlythe.Name = "dataGridView_quanlythe";
            this.dataGridView_quanlythe.ReadOnly = true;
            this.dataGridView_quanlythe.RowHeadersWidth = 51;
            this.dataGridView_quanlythe.RowTemplate.Height = 24;
            this.dataGridView_quanlythe.Size = new System.Drawing.Size(830, 336);
            this.dataGridView_quanlythe.TabIndex = 0;
            this.dataGridView_quanlythe.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView_quanlythe_CellClick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(925, 518);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Thông tin thẻ";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.button_xoa);
            this.groupBox2.Controls.Add(this.button_them);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.textBox_id);
            this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox2.Location = new System.Drawing.Point(202, 367);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(455, 145);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Chỉnh sửa thông tin";
            // 
            // button_xoa
            // 
            this.button_xoa.Location = new System.Drawing.Point(314, 89);
            this.button_xoa.Name = "button_xoa";
            this.button_xoa.Size = new System.Drawing.Size(114, 41);
            this.button_xoa.TabIndex = 3;
            this.button_xoa.Text = "Xóa";
            this.button_xoa.UseVisualStyleBackColor = true;
            this.button_xoa.Click += new System.EventHandler(this.button_xoa_Click);
            // 
            // button_them
            // 
            this.button_them.Location = new System.Drawing.Point(314, 31);
            this.button_them.Name = "button_them";
            this.button_them.Size = new System.Drawing.Size(114, 41);
            this.button_them.TabIndex = 2;
            this.button_them.Text = "Thêm";
            this.button_them.UseVisualStyleBackColor = true;
            this.button_them.Click += new System.EventHandler(this.button_them_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(39, 37);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 22);
            this.label1.TabIndex = 1;
            this.label1.Text = "ID:";
            // 
            // textBox_id
            // 
            this.textBox_id.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_id.Location = new System.Drawing.Point(100, 37);
            this.textBox_id.Name = "textBox_id";
            this.textBox_id.Size = new System.Drawing.Size(137, 28);
            this.textBox_id.TabIndex = 0;
            // 
            // Quanlythe
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
            this.ClientSize = new System.Drawing.Size(949, 542);
            this.Controls.Add(this.dataGridView_quanlythe);
            this.Controls.Add(this.groupBox1);
            this.MaximizeBox = false;
            this.Name = "Quanlythe";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Quanlythe";
            this.Load += new System.EventHandler(this.Quanlythe_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_quanlythe)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dataGridView_quanlythe;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_id;
        private System.Windows.Forms.Button button_xoa;
        private System.Windows.Forms.Button button_them;
    }
}