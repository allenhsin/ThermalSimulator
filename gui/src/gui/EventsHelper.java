package gui;

/**
 * Simple class that helps with events handling, designed to be extended by
 * Classes implementing Listeners
 * @param <T> the owner class
 */
abstract class EventsHelper<T>
{
	protected T owner;
	EventsHelper(T owner)
	{
		this.owner = owner;
	}
}