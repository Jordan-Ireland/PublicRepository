using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SunMoon : MonoBehaviour
{
    public Light sun;
    public Light moon;

    public float y, z = 25;

    // Start is called before the first frame update
    void Start()
    {
        moon.enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        transform.rotation = Quaternion.Euler(GameManager.TimeOfDay, y, z);
        if(GameManager.TimeOfDay >= 179 && GameManager.TimeOfDay < 359)
        {
            sun.enabled = false;
            moon.enabled = true;
        }else if(GameManager.TimeOfDay >= 359)
        {
            sun.enabled = true;
            moon.enabled = false;
        }
    }
}
