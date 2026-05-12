namespace SmartCarPark
{
    partial class Lichsu
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.textBox_time_ra = new System.Windows.Forms.TextBox();
            this.dataGridView_out = new System.Windows.Forms.DataGridView();
            this.textBox_bienso_ra = new System.Windows.Forms.TextBox();
            this.textBox_id_ra = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.textBox_time_vao = new System.Windows.Forms.TextBox();
            this.textBox_bienso_vao = new System.Windows.Forms.TextBox();
            this.textBox_id_vao = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.dataGridView_in = new System.Windows.Forms.DataGridView();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_out)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_in)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.groupBox3);
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.Location = new System.Drawing.Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(1105, 570);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Lịch sử";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.textBox_time_ra);
            this.groupBox3.Controls.Add(this.dataGridView_out);
            this.groupBox3.Controls.Add(this.textBox_bienso_ra);
            this.groupBox3.Controls.Add(this.textBox_id_ra);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Location = new System.Drawing.Point(479, 26);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(619, 538);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Lịch sử ra";
            // 
            // textBox_time_ra
            // 
            this.textBox_time_ra.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_time_ra.Location = new System.Drawing.Point(191, 497);
            this.textBox_time_ra.Name = "textBox_time_ra";
            this.textBox_time_ra.ReadOnly = true;
            this.textBox_time_ra.Size = new System.Drawing.Size(247, 28);
            this.textBox_time_ra.TabIndex = 14;
            // 
            // dataGridView_out
            // 
            this.dataGridView_out.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView_out.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_out.Location = new System.Drawing.Point(6, 31);
            this.dataGridView_out.Name = "dataGridView_out";
            this.dataGridView_out.RowHeadersWidth = 51;
            this.dataGridView_out.RowTemplate.Height = 24;
            this.dataGridView_out.Size = new System.Drawing.Size(622, 372);
            this.dataGridView_out.TabIndex = 0;
            this.dataGridView_out.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView_out_CellClick);
            // 
            // textBox_bienso_ra
            // 
            this.textBox_bienso_ra.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_bienso_ra.Location = new System.Drawing.Point(191, 460);
            this.textBox_bienso_ra.Name = "textBox_bienso_ra";
            this.textBox_bienso_ra.ReadOnly = true;
            this.textBox_bienso_ra.Size = new System.Drawing.Size(247, 28);
            this.textBox_bienso_ra.TabIndex = 13;
            // 
            // textBox_id_ra
            // 
            this.textBox_id_ra.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_id_ra.Location = new System.Drawing.Point(191, 423);
            this.textBox_id_ra.Name = "textBox_id_ra";
            this.textBox_id_ra.ReadOnly = true;
            this.textBox_id_ra.Size = new System.Drawing.Size(247, 28);
            this.textBox_id_ra.TabIndex = 12;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(71, 428);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(32, 22);
            this.label6.TabIndex = 9;
            this.label6.Text = "ID:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(71, 500);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(90, 22);
            this.label4.TabIndex = 11;
            this.label4.Text = "Thời gian:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(71, 464);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(75, 22);
            this.label5.TabIndex = 10;
            this.label5.Text = "Biển số:";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.textBox_time_vao);
            this.groupBox2.Controls.Add(this.textBox_bienso_vao);
            this.groupBox2.Controls.Add(this.textBox_id_vao);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.dataGridView_in);
            this.groupBox2.Location = new System.Drawing.Point(9, 26);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(464, 538);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Lịch sử vào";
            // 
            // textBox_time_vao
            // 
            this.textBox_time_vao.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_time_vao.Location = new System.Drawing.Point(182, 498);
            this.textBox_time_vao.Name = "textBox_time_vao";
            this.textBox_time_vao.ReadOnly = true;
            this.textBox_time_vao.Size = new System.Drawing.Size(247, 28);
            this.textBox_time_vao.TabIndex = 8;
            // 
            // textBox_bienso_vao
            // 
            this.textBox_bienso_vao.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_bienso_vao.Location = new System.Drawing.Point(182, 461);
            this.textBox_bienso_vao.Name = "textBox_bienso_vao";
            this.textBox_bienso_vao.ReadOnly = true;
            this.textBox_bienso_vao.Size = new System.Drawing.Size(247, 28);
            this.textBox_bienso_vao.TabIndex = 7;
            // 
            // textBox_id_vao
            // 
            this.textBox_id_vao.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox_id_vao.Location = new System.Drawing.Point(182, 424);
            this.textBox_id_vao.Name = "textBox_id_vao";
            this.textBox_id_vao.ReadOnly = true;
            this.textBox_id_vao.Size = new System.Drawing.Size(247, 28);
            this.textBox_id_vao.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(62, 501);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(90, 22);
            this.label3.TabIndex = 5;
            this.label3.Text = "Thời gian:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(62, 465);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(75, 22);
            this.label2.TabIndex = 4;
            this.label2.Text = "Biển số:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(62, 429);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 22);
            this.label1.TabIndex = 3;
            this.label1.Text = "ID:";
            // 
            // dataGridView_in
            // 
            this.dataGridView_in.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView_in.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_in.Location = new System.Drawing.Point(6, 31);
            this.dataGridView_in.Name = "dataGridView_in";
            this.dataGridView_in.RowHeadersWidth = 51;
            this.dataGridView_in.RowTemplate.Height = 24;
            this.dataGridView_in.Size = new System.Drawing.Size(452, 372);
            this.dataGridView_in.TabIndex = 1;
            this.dataGridView_in.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView_in_CellClick);
            // 
            // Lichsu
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
            this.ClientSize = new System.Drawing.Size(1113, 582);
            this.Controls.Add(this.groupBox1);
            this.Name = "Lichsu";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Lichsu";
            this.Load += new System.EventHandler(this.Lichsu_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_out)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_in)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.DataGridView dataGridView_out;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox textBox_id_vao;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.DataGridView dataGridView_in;
        private System.Windows.Forms.TextBox textBox_time_ra;
        private System.Windows.Forms.TextBox textBox_bienso_ra;
        private System.Windows.Forms.TextBox textBox_id_ra;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox_time_vao;
        private System.Windows.Forms.TextBox textBox_bienso_vao;
    }
}