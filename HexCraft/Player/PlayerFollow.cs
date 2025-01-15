using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerFollow : MonoBehaviour
{
    float xRot = 0;
    Vector3 initialPosition = new Vector3(0, 0.5f, 0.2f);

    // Use this for initialization
    void Start()
    {

    }

    void Update()
    {
        if (GameManager.gameState == GameManager.GameState.Exploring)
        {
            if (UIManager.instance.moveCamera && !UIManager.ShowingMenu())
            {
                xRot -= Input.GetAxis("Mouse Y") * InputManager.MouseYSpeed;
                xRot = ClampAngle(xRot, -50, 60);
                Vector3 rotation = new Vector3(xRot, GameManager.playerInstance.transform.eulerAngles.y, 0);
                transform.rotation = Quaternion.Euler(rotation);
                transform.localPosition = initialPosition;
            }
        }else if(GameManager.gameState == GameManager.GameState.MovingToExplore)
        {
            transform.localPosition = Vector3.Lerp(transform.localPosition, initialPosition, Time.deltaTime * 15);
            transform.rotation = Quaternion.Lerp(transform.rotation, GameManager.playerInstance.transform.rotation, Time.deltaTime * 15);
            if (Vector3.Distance(transform.localPosition, initialPosition) <= 0.1f)
            {
                transform.localPosition = initialPosition;
                transform.rotation = GameManager.playerInstance.transform.rotation;
                GameManager.gameState = GameManager.GameState.Exploring;
            }
        }
    }

    private void LateUpdate()
    {

    }

    public static float ClampAngle(float angle, float min, float max)
    {
        if (angle < -360F)
            angle += 360F;
        if (angle > 360F)
            angle -= 360F;
        return Mathf.Clamp(angle, min, max);
    }
}
