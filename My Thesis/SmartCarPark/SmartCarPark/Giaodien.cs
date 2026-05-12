using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Security.AccessControl;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AForge.Video.DirectShow;
using Emgu.CV;
using Emgu.CV.CvEnum;
using Emgu.CV.Structure;
using tesseract;


namespace SmartCarPark
{
    public partial class Giaodien : Form
    {
        static StringBuilder dataBuffer = new StringBuilder();
        public static string id_ls_vao;
        public static string id_ls_ra;
        public static string bienso_ls_vao;
        public static string bienso_ls_ra;
        public static string time_ls_vao;
        public static string time_ls_ra;
        private static bool isOpen_in = false;
        private static bool isOpen_out = false;
        private int slot;

        private FilterInfoCollection videoDevices;
        private Capture _capture;

        List<Image<Bgr, byte>> PlateImagesList = new List<Image<Bgr, byte>>();
        Image Plate_Draw;
        List<string> PlateTextList = new List<string>();
        List<Rectangle> listRect = new List<Rectangle>();
        PictureBox[] box = new PictureBox[12];

        public TesseractProcessor full_tesseract = null;
        public TesseractProcessor ch_tesseract = null;
        public TesseractProcessor num_tesseract = null;
        private string m_path = Application.StartupPath + @"\data\";
        private List<string> lstimages = new List<string>();
        private const string m_lang = "eng";


        public Giaodien()
        {
            InitializeComponent();
            GetVideoDevices();
            InitializeTimer();
            this.FormClosing += new FormClosingEventHandler(Giaodien_FormClosing);
        }
        private void InitializeTimer()
        {
            Timer timer = new Timer();
            timer.Interval = 1000;
            timer.Tick += Timer_Tick;
            timer.Start();
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            DateTime currentTime = DateTime.Now;
            label_clock.Text = currentTime.ToString("HH:mm:ss tt");
        }


        private void Giaodien_Load(object sender, EventArgs e)
        {
            //_capture = new Emgu.CV.Capture();
            full_tesseract = new TesseractProcessor();
            bool succeed = full_tesseract.Init(m_path, m_lang, 3);
            if (!succeed)
            {
                MessageBox.Show("Tesseract initialization failed. The application will exit.");
                Application.Exit();
            }
            full_tesseract.SetVariable("tessedit_char_whitelist", "ABCDEFHKLMNPRSTVXY1234567890").ToString();

            ch_tesseract = new TesseractProcessor();
            succeed = ch_tesseract.Init(m_path, m_lang, 3);
            if (!succeed)
            {
                MessageBox.Show("Tesseract initialization failed. The application will exit.");
                Application.Exit();
            }
            ch_tesseract.SetVariable("tessedit_char_whitelist", "ABCDEFHKLMNPRSTUVXY").ToString();

            num_tesseract = new TesseractProcessor();
            succeed = num_tesseract.Init(m_path, m_lang, 3);
            if (!succeed)
            {
                MessageBox.Show("Tesseract initialization failed. The application will exit.");
                Application.Exit();
            }
            num_tesseract.SetVariable("tessedit_char_whitelist", "1234567890").ToString();


            m_path = System.Environment.CurrentDirectory + "\\";
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                comboBox_port_in.Items.Add(port);
                comboBox_port_out.Items.Add(port);
            }

            for (int i = 0; i < box.Length; i++)
            {
                box[i] = new PictureBox();
            }

        }
        private void Giaodien_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_capture != null)
            {
                _capture.Stop();
                _capture.Dispose();
            }
            try
            {
                serialPort1.Close();
                serialPort2.Close();
            }
            finally
            {
                serialPort1.Dispose();
                serialPort2.Dispose();
            }

        }
        private void GetVideoDevices()
        {
            videoDevices = new FilterInfoCollection(FilterCategory.VideoInputDevice);
            foreach (FilterInfo device in videoDevices)
            {
                comboBox_cam.Items.Add(device.Name);
            }
        }

        private void button_cam_Click(object sender, EventArgs e)
        {
            if (_capture != null)
            {
                _capture.Stop();
                _capture.Dispose();
            }
            int selectedCameraIndex = comboBox_cam.SelectedIndex;
            if (selectedCameraIndex < 0)
            {
                MessageBox.Show("Please select a camera.");
                return;
            }
            _capture = new Capture(selectedCameraIndex);
            _capture.ImageGrabbed += ProcessFrame;
            _capture.Start();
        }

        private bool _processingFrame = false;

        private void ProcessFrame(object sender, EventArgs e)
        {
            if (_processingFrame) return; // Avoid re-entering if already processing a frame

            _processingFrame = true;

            try
            {
                Image<Bgr, Byte> imageFrame = _capture.QueryFrame(); // Use QueryFrame for Emgu CV 2.4
                if (imageFrame != null)
                {
                    pictureBox_cam.Image = imageFrame.Bitmap; // Display the frame in an ImageBox
                }
            }
            finally
            {
                _processingFrame = false;
            }
        }

        private void button_capture_Click(object sender, EventArgs e)
        {
            if (_capture != null)
            {
                pictureBox_cap_in.Image = null;
                _capture.QueryFrame().Save("aa.bmp");
                FileStream fs = new FileStream(m_path + "aa.bmp", FileMode.Open, FileAccess.Read);
                Image temp = Image.FromStream(fs);
                fs.Close();
                pictureBox_cap_in.Image = temp;
                pictureBox_cap_in.Update();
                Image temp1;
                string temp2, temp3;
                Image img1, img2;
                Reconize(m_path + "aa.bmp", out temp1, out temp2, out temp3, out img1, out img2);
                pictureBox_reg_in.Image = img1;
                pictureBox_blackwhite_in.Image = img2;
                pictureBox_contours_in.Image = temp1;
                if (temp3.Replace("\n", "").Replace("\r", "").Length < 7)
                    textBox_plate_in.Text = "Không nhận diện được biển số";
                else
                    textBox_plate_in.Text = temp3;
            }
        }
        public void ProcessImage(string urlImage)
        {
            PlateImagesList.Clear();
            PlateTextList.Clear();
            FileStream fs = new FileStream(urlImage, FileMode.Open, FileAccess.Read);
            Image img = Image.FromStream(fs);
            Bitmap image = new Bitmap(img);
            fs.Close();
            FindLicensePlate(image, out Plate_Draw);

        }
        public static Bitmap RotateImage(Image image, float angle)
        {
            if (image == null)
                throw new ArgumentNullException("image");

            PointF offset = new PointF((float)image.Width / 2, (float)image.Height / 2);

            //create a new empty bitmap to hold rotated image
            Bitmap rotatedBmp = new Bitmap(image.Width, image.Height);
            rotatedBmp.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            //make a graphics object from the empty bitmap
            Graphics g = Graphics.FromImage(rotatedBmp);

            //Put the rotation point in the center of the image
            g.TranslateTransform(offset.X, offset.Y);

            //rotate the image
            g.RotateTransform(angle);

            //move the image back
            g.TranslateTransform(-offset.X, -offset.Y);

            //draw passed in image onto graphics object
            g.DrawImage(image, new PointF(0, 0));

            return rotatedBmp;
        }
        private string Ocr(Bitmap image_s, bool isFull, bool isNum = false)
        {
            string temp = "";
            Image<Gray, byte> src = new Image<Gray, byte>(image_s);
            double ratio = 1;

            while (true)
            {
                ratio = (double)CvInvoke.cvCountNonZero(src) / (src.Width * src.Height);
                if (ratio > 0.5) break;
                src = src.Dilate(2);
            }
            Bitmap image = src.ToBitmap();

            TesseractProcessor ocr;
            if (isFull)
                ocr = full_tesseract;
            else if (isNum)
                ocr = num_tesseract;
            else
                ocr = ch_tesseract;

            int cou = 0;
            ocr.Clear();
            ocr.ClearAdaptiveClassifier();
            temp = ocr.Apply(image);
            while (temp.Length > 3)
            {
                Image<Gray, byte> temp2 = new Image<Gray, byte>(image);
                temp2 = temp2.Erode(2);
                image = temp2.ToBitmap();
                ocr.Clear();
                ocr.ClearAdaptiveClassifier();
                temp = ocr.Apply(image);
                cou++;
                if (cou > 10)
                {
                    temp = "";
                    break;
                }
            }
            return temp;

        }
        public void FindLicensePlate(Bitmap image, out Image plateDraw)
        {
            plateDraw = null;
            Image<Bgr, byte> frame;
            bool isface = false;
            Bitmap src;
            Image dst = image;
            HaarCascade haar = new HaarCascade(Application.StartupPath + "\\output-hv-33-x25.xml");
            for (float i = 0; i <= 20; i = i + 3)
            {
                for (float s = -1; s <= 1 && s + i != 1; s += 2)
                {
                    src = RotateImage(dst, i * s);
                    PlateImagesList.Clear();
                    frame = new Image<Bgr, byte>(src);
                    using (Image<Gray, byte> grayframe = new Image<Gray, byte>(src))
                    {
                        var faces =
                       grayframe.DetectHaarCascade(haar, 1.1, 8, HAAR_DETECTION_TYPE.DO_CANNY_PRUNING, new Size(0, 0))[0];
                        foreach (var face in faces)
                        {
                            Image<Bgr, byte> tmp = frame.Copy();
                            tmp.ROI = face.rect;

                            frame.Draw(face.rect, new Bgr(Color.Blue), 2);

                            PlateImagesList.Add(tmp);

                            isface = true;
                        }
                        if (isface)
                        {
                            Image<Bgr, byte> showimg = frame.Clone();
                            plateDraw = (Image)showimg.ToBitmap();
                            if (PlateImagesList.Count > 1)
                            {
                                for (int k = 1; k < PlateImagesList.Count; k++)
                                {
                                    if (PlateImagesList[0].Width < PlateImagesList[k].Width)
                                    {
                                        PlateImagesList[0] = PlateImagesList[k];
                                    }
                                }
                            }
                            PlateImagesList[0] = PlateImagesList[0].Resize(400, 400, Emgu.CV.CvEnum.INTER.CV_INTER_LINEAR);
                            return;
                        }


                    }
                }
            }
        }
        private void Reconize(string link, out Image hinhbienso, out string bienso, out string bienso_text, out Image img1, out Image img2)
        {
            for (int i = 0; i < box.Length; i++)
            {
                this.Controls.Remove(box[i]);
            }

            hinhbienso = null;
            img1 = null;
            img2 = null;
            bienso = "";
            bienso_text = "";
            ProcessImage(link);
            if (PlateImagesList.Count != 0)
            {
                Image<Bgr, byte> src = new Image<Bgr, byte>(PlateImagesList[0].ToBitmap());
                Bitmap grayframe;
                FindContours con = new FindContours();
                Bitmap color;
                int c = con.IdentifyContours(src.ToBitmap(), 50, false, out grayframe, out color, out listRect);
                img1 = color;
                hinhbienso = Plate_Draw;
                img2 = grayframe;
                Image<Gray, byte> dst = new Image<Gray, byte>(grayframe);
                grayframe = dst.ToBitmap();
                string zz = "";

                // lọc và sắp xếp số
                List<Bitmap> bmp = new List<Bitmap>();
                List<int> erode = new List<int>();
                List<Rectangle> up = new List<Rectangle>();
                List<Rectangle> dow = new List<Rectangle>();
                int up_y = 0, dow_y = 0;
                bool flag_up = false;

                int di = 0;

                if (listRect == null) return;

                for (int i = 0; i < listRect.Count; i++)
                {
                    Bitmap ch = grayframe.Clone(listRect[i], grayframe.PixelFormat);
                    int cou = 0;
                    full_tesseract.Clear();
                    full_tesseract.ClearAdaptiveClassifier();
                    string temp = full_tesseract.Apply(ch);
                    while (temp.Length > 3)
                    {
                        Image<Gray, byte> temp2 = new Image<Gray, byte>(ch);
                        temp2 = temp2.Erode(2);
                        ch = temp2.ToBitmap();
                        full_tesseract.Clear();
                        full_tesseract.ClearAdaptiveClassifier();
                        temp = full_tesseract.Apply(ch);
                        cou++;
                        if (cou > 10)
                        {
                            listRect.RemoveAt(i);
                            i--;
                            di = 0;
                            break;
                        }
                        di = cou;
                    }
                }

                for (int i = 0; i < listRect.Count; i++)
                {
                    for (int j = i; j < listRect.Count; j++)
                    {
                        if (listRect[i].Y > listRect[j].Y + 100)
                        {
                            flag_up = true;
                            up_y = listRect[j].Y;
                            dow_y = listRect[i].Y;
                            break;
                        }
                        else if (listRect[j].Y > listRect[i].Y + 100)
                        {
                            flag_up = true;
                            up_y = listRect[i].Y;
                            dow_y = listRect[j].Y;
                            break;
                        }
                        if (flag_up == true) break;
                    }
                }

                for (int i = 0; i < listRect.Count; i++)
                {
                    if (listRect[i].Y < up_y + 50 && listRect[i].Y > up_y - 50)
                    {
                        up.Add(listRect[i]);
                    }
                    else if (listRect[i].Y < dow_y + 50 && listRect[i].Y > dow_y - 50)
                    {
                        dow.Add(listRect[i]);
                    }
                }

                if (flag_up == false) dow = listRect;

                for (int i = 0; i < up.Count; i++)
                {
                    for (int j = i; j < up.Count; j++)
                    {
                        if (up[i].X > up[j].X)
                        {
                            Rectangle w = up[i];
                            up[i] = up[j];
                            up[j] = w;
                        }
                    }
                }
                for (int i = 0; i < dow.Count; i++)
                {
                    for (int j = i; j < dow.Count; j++)
                    {
                        if (dow[i].X > dow[j].X)
                        {
                            Rectangle w = dow[i];
                            dow[i] = dow[j];
                            dow[j] = w;
                        }
                    }
                }

                int x = 12;
                int c_x = 0;

                for (int i = 0; i < up.Count; i++)
                {
                    Bitmap ch = grayframe.Clone(up[i], grayframe.PixelFormat);
                    Bitmap o = ch;
                    //ch = con.Erodetion(ch);
                    string temp;
                    if (i < 2)
                    {
                        temp = Ocr(ch, false, true); // nhan dien so
                    }
                    else
                    {
                        temp = Ocr(ch, false, false);// nhan dien chu
                    }

                    zz += temp;
                    box[i].Location = new Point(x + i * 50, 290);
                    box[i].Size = new Size(50, 100);
                    box[i].SizeMode = PictureBoxSizeMode.StretchImage;
                    box[i].Image = ch;
                    box[i].Update();
                    c_x++;
                }
                zz += "\r\n";
                for (int i = 0; i < dow.Count; i++)
                {
                    Bitmap ch = grayframe.Clone(dow[i], grayframe.PixelFormat);
                    //ch = con.Erodetion(ch);
                    string temp = Ocr(ch, false, true); // nhan dien so
                    zz += temp;
                    box[i + c_x].Location = new Point(x + i * 50, 390);
                    box[i + c_x].Size = new Size(50, 100);
                    box[i + c_x].SizeMode = PictureBoxSizeMode.StretchImage;
                    box[i + c_x].Image = ch;
                    box[i + c_x].Update();
                }
                bienso = zz.Replace("\n", "");
                bienso = bienso.Replace("\r", "");
                bienso_text = zz;

            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            bool plate_temp = false;
            string state = null;
            
            Quanlythe quanlythe = new Quanlythe();
            Lichsu lichsu = new Lichsu();
            Xetrongbai xetrongbai = new Xetrongbai();

            int bytesToRead = serialPort1.BytesToRead;
            byte[] buffer = new byte[bytesToRead];
            serialPort1.Read(buffer, 0, bytesToRead);
            string receivedData = Encoding.ASCII.GetString(buffer);
            dataBuffer.Append(receivedData);
            // Xử lý dữ liệu khi bạn nhận được một chuỗi hoàn chỉnh
            if (dataBuffer.ToString().Contains("\0"))
            {
                if (dataBuffer.ToString().StartsWith("@"))
                {
                    state = dataBuffer.ToString();
                    slot = xetrongbai.Xecontrong(state);
                    Invoke(new Action(() =>
                    {
                        textBox_sovitri.Text = slot.ToString();
                    }));
                    if (slot == 0)
                        MessageBox.Show("Bãi đỗ xe đã đầy", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    dataBuffer.Clear();
                }
                else 
                {
                    if (slot > 0)
                    {
                        string id_in = dataBuffer.ToString();
                        Invoke(new Action(() =>
                        {
                            textBox_id_in.Text = id_in;
                        }));
                        string id_compare_in = textBox_id_in.Text;
                        quanlythe.CompareID_In(id_compare_in);

                        // Xử lý dữ liệu ở đây
                        try
                        {
                            Invoke(new Action(() =>
                            {
                                textBox_id_in.Text = Quanlythe.id_in_compared;
                                textBox_time_in.Text = DateTime.Now.ToString();
                            }));

                            if (_capture != null)
                            {
                                pictureBox_cap_in.Image = null;
                                _capture.QueryFrame().Save("aa.bmp");
                                FileStream fs = new FileStream(m_path + "aa.bmp", FileMode.Open, FileAccess.Read);
                                Image temp = Image.FromStream(fs);
                                fs.Close();
                                pictureBox_cap_in.Image = temp;
                                Invoke(new Action(() =>
                                {
                                    pictureBox_cap_in.Update();

                                }));

                                //pictureBox_cap_in.Update();
                                Image temp1;
                                string temp2, temp3;
                                Image img1, img2;
                                Reconize(m_path + "aa.bmp", out temp1, out temp2, out temp3, out img1, out img2);
                                pictureBox_contours_in.Image = temp1;
                                pictureBox_reg_in.Image = img1;
                                pictureBox_blackwhite_in.Image = img2;
                                Invoke(new Action(() =>
                                {
                                    if (temp3.Replace("\n", "").Replace("\r", "").Length < 7)
                                        textBox_plate_in.Text = "Không nhận diện được biển số";
                                    else
                                    {
                                        textBox_plate_in.Text = temp3;
                                        plate_temp = true;
                                    }
                                }));
                            }
                            if (Quanlythe.id_in_temp == true && plate_temp == true)
                            {
                                id_ls_vao = textBox_id_in.Text;
                                bienso_ls_vao = textBox_plate_in.Text.Replace("\n", "").Replace("\r", "");
                                time_ls_vao = textBox_time_in.Text;
                                lichsu.Lichsu_vao();
                                serialPort1.Write("Y");
                            }
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("Lỗi: " + ex.Message);
                        }
                        dataBuffer.Clear();
                    }
                }
            }
        }

        private void serialPort2_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            bool plate_temp = false;

            Quanlythe quanlythe = new Quanlythe();
            Lichsu lichsu = new Lichsu();

            int bytesToRead = serialPort2.BytesToRead;
            byte[] buffer = new byte[bytesToRead];
            serialPort2.Read(buffer, 0, bytesToRead);
            string receivedData = Encoding.ASCII.GetString(buffer);
            dataBuffer.Append(receivedData);
            // Xử lý dữ liệu khi bạn nhận được một chuỗi hoàn chỉnh
            if (dataBuffer.ToString().Contains("\0"))
            {
                string id_out = dataBuffer.ToString();
                Invoke(new Action(() =>
                {
                    textBox_id_out.Text = id_out;
                }));
                string id_compare_out = textBox_id_out.Text;
                quanlythe.CompareID_Out(id_compare_out);
                // Xử lý dữ liệu ở đây
                try
                {
                    Invoke(new Action(() =>
                    {
                        textBox_id_out.Text = Quanlythe.id_out_compared;
                        textBox_time_out.Text = DateTime.Now.ToString();
                    }));

                    if (_capture != null)
                    {
                        pictureBox_cap_out.Image = null;
                        _capture.QueryFrame().Save("aaa.bmp");
                        FileStream fs = new FileStream(m_path + "aaa.bmp", FileMode.Open, FileAccess.Read);
                        Image temp = Image.FromStream(fs);
                        fs.Close();
                        pictureBox_cap_out.Image = temp;
                        Invoke(new Action(() =>
                        {
                            pictureBox_cap_out.Update();

                        }));

                        Image temp1;
                        string temp2, temp3;
                        Image img1, img2;
                        Reconize(m_path + "aaa.bmp", out temp1, out temp2, out temp3, out img1, out img2);
                        pictureBox_contours_out.Image = temp1;
                        pictureBox_reg_out.Image = img1;
                        pictureBox_blackwhite_out.Image = img2;
                        Invoke(new Action(() =>
                        {
                            if (temp3.Replace("\n", "").Replace("\r", "").Length < 7)
                                textBox_plate_out.Text = "Không nhận diện được biển số";
                            else
                            {
                                textBox_plate_out.Text = temp3;
                                plate_temp = true;
                            }
                        }));
                    }
                    if (Quanlythe.id_out_temp == true && plate_temp == true)
                    {
                        id_ls_ra = textBox_id_out.Text;
                        bienso_ls_ra = textBox_plate_out.Text.Replace("\n", "").Replace("\r", "");
                        time_ls_ra = textBox_time_out.Text;
                        lichsu.Lichsu_ra();
                        if (Lichsu.match == true)
                        {
                            serialPort2.Write("Y");
                            Invoke(new Action(() =>
                            {
                                textBox_phiguixe.Text = Doanhthu.phiguixe;
                                textBox_time_guixe.Text = Lichsu.time_guixe.ToString();
                            }));
                        }
                        else
                        {
                            MessageBox.Show("Lỗi: Thẻ hoặc biển số không có trong hệ thống!!!", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Lỗi: " + ex.Message);
                }
                dataBuffer.Clear();
            }
        }

        private void comboBox_port_in_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort1.Close();
            serialPort1.PortName = comboBox_port_in.Text;
            button_port_in.Enabled = true;
        }

        private void comboBox_port_out_SelectedIndexChanged(object sender, EventArgs e)
        {
            serialPort2.Close();
            serialPort2.PortName = comboBox_port_out.Text;
            button_port_out.Enabled = true;
        }
        private void button_port_in_Click(object sender, EventArgs e)
        {
            if (comboBox_port_in.Text == "")
            {
                MessageBox.Show("Select COM Port", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);

            }
            else
            {
                try
                {
                    if (serialPort1.IsOpen) // Xem cong COM da co thiet bi ket noi chua neu co roi thi khong duoc chon nua
                    {
                        MessageBox.Show("COM Port is connected and ready to use", "Information", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
                    }
                    else
                    {
                        serialPort1.Open();
                        button_port_in.Enabled = false;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("COM Port is not found", "Error", MessageBoxButtons.OKCancel, MessageBoxIcon.Error);
                }
            }
        }

        private void button_port_out_Click(object sender, EventArgs e)
        {
            if (comboBox_port_out.Text == "")
            {
                MessageBox.Show("Select COM Port", "Warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);

            }
            else
            {
                try
                {
                    if (serialPort2.IsOpen) // Xem cong COM da co thiet bi ket noi chua neu co roi thi khong duoc chon nua
                    {
                        MessageBox.Show("COM Port is connected and ready to use", "Information", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);

                    }
                    else
                    {
                        serialPort2.Open();
                        button_port_out.Enabled = false;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("COM Port is not found", "Error", MessageBoxButtons.OKCancel, MessageBoxIcon.Error);
                }
            }
        }

        private void quảnLýThẻToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Quanlythe qlthe = new Quanlythe();
            qlthe.Show();
        }

        private void lịchSửToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Lichsu lichsu = new Lichsu();
            lichsu.Show();
        }

        private void doanhThuToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Doanhthu doanhthu = new Doanhthu();
            doanhthu.Show();
        }
        private void vịTríĐỗXeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Xetrongbai xetrongbai = new Xetrongbai();
            xetrongbai.Show();
        }

        private void button_barie_in_Click(object sender, EventArgs e)
        {
            try
            {
                if(!isOpen_in)
                {
                    serialPort1.Write("O");
                    isOpen_in = true;
                    button_barie_in.Text = "Đóng cửa";
                }
                else
                {
                    serialPort1.Write("C");
                    isOpen_in = false;
                    button_barie_in.Text = "Mở cửa";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Lỗi: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void button_barie_out_Click(object sender, EventArgs e)
        {
            try
            {
                if (!isOpen_out)
                {
                    serialPort2.Write("O");
                    isOpen_out = true;
                    button_barie_out.Text = "Đóng cửa";
                }
                else
                {
                    serialPort2.Write("C");
                    isOpen_out = false;
                    button_barie_out.Text = "Mở cửa";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Lỗi: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void reset_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.Write("R");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Lỗi: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }
    }

}