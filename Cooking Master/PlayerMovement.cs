using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// class to hold player movement
/// </summary>
public class PlayerMovement : MonoBehaviour
{
    public KeyCode up, down, left, right, interact;
    float Vertical
    {
        get
        {
            float value = 0;
            if (Input.GetKey(up))
                value = 1;
            else if (Input.GetKey(down))
                value = -1;
            return value;
        }
    }
    float Horizontal
    {
        get
        {
            float value = 0;
            if (Input.GetKey(right))
                value = 1;
            else if (Input.GetKey(left))
                value = -1;
            return value;
        }
    }
    Rigidbody2D rb;
    public float movementMultiplier = 1;
    float timeSince = 0;

    public bool canMove = true;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        if (movementMultiplier > 1)
        {
            timeSince += Time.deltaTime;
            if (timeSince >= 15)
            {
                movementMultiplier = 1;
            }
        }

        if (canMove)
        {
            Vector2 input = new Vector2(Horizontal, Vertical);
            rb.velocity = input * 5 * movementMultiplier;
        }else
        {
            rb.velocity = Vector2.zero;
        }
    }
}
