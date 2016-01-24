package ch.santos_alves.nitroxanalyzer;

import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import ch.santos_alves.nitroxanalyzer.activities.OxygenDisplay;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.doesNotExist;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

/**
 * Created by Sergio on 17.12.2015.
 */
@RunWith(AndroidJUnit4.class)
public class OxygenDisplayTest {

    @Rule
    public ActivityTestRule<OxygenDisplay> mActivityRule = new ActivityTestRule<>(OxygenDisplay.class);

    public OxygenDisplayTest() {
        super();
    }

    @Test
    public void listGoesOverTheFold() {
        onView(withId(R.id.imageButton)).check(matches(isDisplayed()));
        onView(withId(R.id.btnGetO2)).check(matches(isDisplayed()));

        SharedPreferences sPrefs = PreferenceManager.getDefaultSharedPreferences(mActivityRule.getActivity().getApplicationContext());

        if (sPrefs.getBoolean("continuous_o2_measurement", false))
            onView(withId(R.id.btnGetO2)).check(doesNotExist());
        else
            onView(withId(R.id.btnGetO2)).check(matches(isDisplayed()));

        if (sPrefs.getBoolean("automatic_calibration_at_connection", false))
            onView(withId(R.id.btnCalibrate)).check(doesNotExist());
        else
            onView(withId(R.id.btnCalibrate)).check(matches(isDisplayed()));
    }
}
