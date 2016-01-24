package ch.santos_alves.nitroxanalyzer.views;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.view.View;

import ch.santos_alves.nitroxanalyzer.R;

/**
 * TODO: document your custom view class.
 */
public class OxyGenPercentDisplayer extends View {
    private float oxyrate;
    private TextPaint mTextPaint;
    private int nitrox1stLineStartTextPosition, nitroxBottomTextPosition, nitroxTopTextPosition,
            contentHeight, topPosition, leftPosition, rightPosition, bottomPosition;
    private Drawable tank;

    public OxyGenPercentDisplayer(Context context) {
        super(context);
        init(null, 0);
    }

    public OxyGenPercentDisplayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(attrs, 0);
    }

    public OxyGenPercentDisplayer(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init(attrs, defStyle);
    }

    private void init(AttributeSet attrs, int defStyle) {
        final TypedArray a;

        // Load attributes
        a = getContext().obtainStyledAttributes(attrs, R.styleable.OxyGenPercentDisplayer, defStyle, 0);
        oxyrate = a.getInteger(R.styleable.OxyGenPercentDisplayer_oxyRate, 0);
        a.recycle();

        mTextPaint = new TextPaint();
        mTextPaint.setFlags(Paint.ANTI_ALIAS_FLAG);
        mTextPaint.setTextAlign(Paint.Align.CENTER);
        mTextPaint.setColor(Color.argb(255, 251, 251, 0));

/*
        if(fsize<0 )
            fsize = 0;
        if(fsize > 10 && fsize < 20)
            fsize--;
        if(fsize > 20 && fsize < 40)
            fsize-=2;
    */
    }

    @Override
    public void invalidate() {
        super.invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        tank = getContext().getDrawable(R.drawable.nitroxtank);
        contentHeight = getHeight() - (getPaddingTop() + getPaddingBottom());
        int fSize = 0;

        nitroxTopTextPosition = (int) (getHeight() * (250.0 / 640.0));
        nitrox1stLineStartTextPosition = (int) (getHeight() * (295.0 / 640.0));
        nitroxBottomTextPosition = (int) (getHeight() * (340.0 / 640.0));
        fSize = nitrox1stLineStartTextPosition - nitroxTopTextPosition;
        fSize -= (int) (fSize / 20.0);
        mTextPaint.setTextSize(fSize);

        topPosition = (getHeight() - contentHeight) / 2;
        leftPosition = (getWidth() - contentHeight / 2) / 2;
        rightPosition = leftPosition + contentHeight / 2;
        bottomPosition = topPosition + contentHeight;

        setMinimumWidth(contentHeight / 2);

        tank.setBounds(leftPosition, topPosition, rightPosition, bottomPosition);
        tank.draw(canvas);

        //Draws the 1st string
        canvas.drawText("NITROX", contentHeight / 4 + leftPosition, nitrox1stLineStartTextPosition, mTextPaint);

        //depending on the value draws the second string
        if(oxyrate > 0 )
            canvas.drawText(String.format("EAN%d", (int) Math.ceil(oxyrate)), contentHeight / 4 + leftPosition, nitroxBottomTextPosition, mTextPaint);
        else
            canvas.drawText("EAN---", contentHeight / 4 + leftPosition, nitroxBottomTextPosition, mTextPaint);
    }

    public float getOxyrate() {
        return oxyrate;
    }

    public void setOxyrate(float oxyrate) {
        this.oxyrate = oxyrate;
        invalidate();
    }
}
