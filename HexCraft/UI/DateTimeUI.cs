using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class DateTimeUI : MonoBehaviour
{
    public GameObject SunMoon;
    public GameObject sun;
    public GameObject moon;
    public GameObject timeOfDay;
    TextMeshProUGUI timeOfDayText;

    // Start is called before the first frame update
    void Start()
    {
        timeOfDayText = timeOfDay.GetComponent<TextMeshProUGUI>();
    }

    // Update is called once per frame
    void Update()
    {
        SunMoon.transform.rotation = Quaternion.Euler(0, 0, -GameManager.TimeOfDay);

        sun.transform.rotation = Quaternion.Euler(0, 0, GameManager.TimeOfDay);
        moon.transform.rotation = Quaternion.Euler(0, 0, GameManager.TimeOfDay);

        //timeOfDayText.SetText(Mathf.Round(GameManager.TimeOfDay / 15).ToString());
        timeOfDayText.SetText(GameManager.Day.ToString());
    }
}
