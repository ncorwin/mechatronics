package com.me433_hw12.ncorwin.linecam;

// libraries
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;
import android.widget.TextView;
import java.io.IOException;
import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640,480,Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;

    SeekBar redThresh;
    SeekBar greenThresh;
    SeekBar blueThresh;

    TextView redText;
    TextView greenText;
    TextView blueText;

    static long prevtime = 0; // for FPS calculation

    static double redVal = 0;
    static double greenVal = 0;
    static double blueVal = 0;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
        mSurfaceHolder = mSurfaceView.getHolder();

        mTextureView = (TextureView) findViewById(R.id.textureview);
        mTextureView.setSurfaceTextureListener(this);

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        paint1.setColor(0xffff0000); // red
        paint1.setTextSize(24);

        redThresh = (SeekBar) findViewById(R.id.seekRed);

        redText = (TextView) findViewById(R.id.textRed);
        redText.setText("Red Threshold");

        greenThresh = (SeekBar) findViewById(R.id.seekGreen);

        greenText = (TextView) findViewById(R.id.textGreen);
        greenText.setText("Green Threshold");

        blueThresh = (SeekBar) findViewById(R.id.seekBlue);

        blueText = (TextView) findViewById(R.id.textBlue);
        blueText.setText("Blue Threshold");

        setMyControlListener();
    }

    private void setMyControlListener() {
        redThresh.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int progressChangedRed = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progressRed, boolean fromUser) {
                progressChangedRed = progressRed;
                redVal = progressRed*2.55;
                redText.setText("Red Threshold value: "+redVal);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        greenThresh.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int progressChangedGreen = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progressGreen, boolean fromUser) {
                progressChangedGreen = progressGreen;
                greenVal = progressGreen*2.55;
                greenText.setText("Green Threshold value: "+greenVal);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        blueThresh.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int progressChangedBlue = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progressBlue, boolean fromUser) {
                progressChangedBlue = progressBlue;
                blueVal = progressBlue*2.55;
                blueText.setText("Blue Threshold value: "+blueVal);

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setColorEffect(Camera.Parameters.EFFECT_NONE); // black and white
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {

            int[] pixels = new int[bmp.getWidth()];
            int startY = 15; // which row in the bitmap to analyse to read
            // only look at one row in the image
            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)

            // pixels[] is the RGBA data (in black an white).
            // instead of doing center of mass on it, decide if each pixel is dark enough to consider black or white
            // then do a center of mass on the thresholded array
            int[] thresholdedPixels = new int[bmp.getWidth()];
            int[] thresholdedColors = new int[bmp.getWidth()];
            int wbTotal = 0; // total mass
            int wbCOM = 0; // total (mass time position)
            for (int i = 0; i < bmp.getWidth(); i++) {
                // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                // if it is greater than some value (600 here), consider it black
                // play with the 600 value if you are having issues reliably seeing the line
                //if (255*3-(red(pixels[i])+green(pixels[i])+blue(pixels[i])) > redVal+greenVal+blueVal) {
                if ((red(pixels[i]) > redVal) && (green(pixels[i]) > greenVal) && (blue(pixels[i]) > blueVal)) {
                    thresholdedPixels[i] = 255*3;
                    thresholdedColors[i] = Color.rgb((int)redVal, (int)greenVal, (int)blueVal);
                }
                else {
                    thresholdedPixels[i] = 0;
                }
                wbTotal = wbTotal + thresholdedPixels[i];
                wbCOM = wbCOM + thresholdedPixels[i]*i;
            }
            int COM;
            //watch out for divide by 0
            if (wbTotal<=0) {
                COM = bmp.getWidth()/2;
            }
            else {
                COM = wbCOM/wbTotal;
            }

            // draw a circle where you think the COM is
            canvas.drawCircle(COM, startY, 5, paint1);
            bmp.setPixels(thresholdedColors, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);

            // also write the value as text
            canvas.drawText("COM = " + COM, 10, 200, paint1);
            c.drawBitmap(bmp, 0, 0, null);
            mSurfaceHolder.unlockCanvasAndPost(c);

            // calculate the FPS to see how fast the code is running
            long nowtime = System.currentTimeMillis();
            long diff = nowtime - prevtime;
            mTextView.setText("FPS " + 1000/diff);
            prevtime = nowtime;
        }
    }
}

