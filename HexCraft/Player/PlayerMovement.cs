using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//[RequireComponent(typeof(Rigidbody))]
[RequireComponent(typeof(CharacterController))]
public class PlayerMovement : MonoBehaviour
{
	Rigidbody rb;
	CharacterController controller;
	public bool canMove = true;
	public bool canRotate = true;
	public bool isGrounded = false;
	PlayerFollow _camera;
	float rotY = 0;

	float gravity = -50f;
	Vector3 playerVelocity;
	bool groundedPlayer = true;

	Coroutine staminaco;

	float v, h;

	void Awake()
	{
		rb = GetComponent<Rigidbody>();
		_camera = Camera.main.GetComponent<PlayerFollow>();
		controller = GetComponent<CharacterController>();
	}

	// Use this for initialization
	void Start()
	{

	}

	// Update is called once per frame
	void Update()
	{
		if (GameManager.gameState == GameManager.GameState.Exploring && GameManager.playerInstance.canMove && !UIManager.ShowingMenu())
		{
			groundedPlayer = controller.isGrounded;
			if (groundedPlayer && playerVelocity.y < 0)
			{
				playerVelocity.y = 0f;
			}

			v = InputManager.Vertical;// Input.GetAxisRaw("Vertical");
			h = InputManager.Horizontal;// Input.GetAxisRaw("Horizontal");
			Vector3 input = new Vector3(h * 1, 0, v * 1);
			input.Normalize();
			input = transform.TransformDirection(input);
			controller.Move(input * Time.deltaTime * GameManager.playerInstance.moveSpeed);

			// Changes the height position of the player..
			if (Input.GetKey(InputManager.Jump) && groundedPlayer)
			{
				playerVelocity.y += Mathf.Sqrt(2.5f * -3.0f * gravity);
			}
			if (canRotate)
			{
				Rotate();
			}
		}else
        {
			controller.Move(Vector3.zero);
		}

		playerVelocity.y += gravity * Time.deltaTime;
		controller.Move(playerVelocity * Time.deltaTime);
	}

	void Movement()
	{
		v = InputManager.Vertical;// Input.GetAxisRaw("Vertical");
		h = InputManager.Horizontal;// Input.GetAxisRaw("Horizontal");
		Vector3 input = new Vector3(h * 1, 0, v * 1);
		input.Normalize();

		if (input.z < 0)
		{
			input.z = input.z / 2;
		}
		if (input.x > 0.2f || input.x < -0.2f)
		{
			input.x = input.x / 1.5f;
		}


		input = transform.TransformDirection(input);
		input *= 15;


		input.y = rb.velocity.y;

		if (isGrounded && Input.GetKey(InputManager.Jump))
		{
			input.y = 15;
		}


		rb.velocity = input;
		if (canRotate)
		{
			Rotate();
		}
	}

	void Rotate()
	{
		rotY += Input.GetAxis("Mouse X") * InputManager.MouseXSpeed;
		Vector3 rotation = new Vector3(0, rotY, 0);
		transform.rotation = Quaternion.Euler(rotation);
		//// Get camera forward direction, without vertical component.
		//Vector3 forward = _camera.transform.TransformDirection(Vector3.forward);

		//// Player is moving on ground, Y component of camera facing is not relevant.
		//forward.y = 0.0f;
		//forward = forward.normalized;

		//Quaternion targetRotation = Quaternion.LookRotation(forward);

		//Quaternion newRotation = Quaternion.Slerp(rb.rotation, targetRotation, 0.50f);
		//rb.MoveRotation(newRotation);
	}
}
